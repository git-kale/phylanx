//   Copyright (c) 2018 Shahrzad Shirzad
//   Copyright (c) 2020 Hartmut Kaiser
//   Copyright (c) 2020 Bita Hasheminezhad
//   Copyright (c) 2020 Nanmiao Wu
//
//   Distributed under the Boost Software License, Version 1.0. (See accompanying
//   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <phylanx/phylanx.hpp>
#include <hpx/hpx_init.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <tuple>

#include <blaze/Math.h>
#include <hpx/program_options.hpp>

//////////////////////////////////////////////////////////////////////////////////
// This example uses part of the breast cancer dataset from UCI Machine Learning
// Repository.
//     https://archive.ics.uci.edu/ml/datasets/Breast+Cancer+Wisconsin+(Diagnostic)
//
// A copy of the full dataset in CSV format (breast_cancer.csv), obtained from
// scikit-learn datasets, is provided in the same folder as this example.
//
// The layout of the data in the provided CSV file used by the example
// is as follows:
// 30 features per line followed by the classification
// 569 lines of data
/////////////////////////////////////////////////////////////////////////////////

char const* const read_r_code = R"(block(
    //
    // Read X-data from given CSV file
    //
    define(read_r, filepath, row_start, row_stop, col_start, col_stop,
        annotate_d(
            slice(file_read_csv(filepath),
                list(row_start, row_stop), list(col_start, col_stop)),
            "read_r",
            list("tile",
                list("rows", row_start, row_stop),
                list("columns", col_start, col_stop)
            )
        )
    ),
    read_r
))";

///////////////////////////////////////////////////////////////////////////////
char const* const als_code = R"(
    //
    // Alternating Least squares algorithm
    //
    define(__als, ratings_row, ratings_column, regularization, num_factors,
        iterations, alpha, enable_output,
        block(
            define(num_users, shape(ratings_row, 0)),
            define(total_num_items, shape_d(ratings_row, 1)),
            define(total_num_users, shape_d(ratings_row, 0)),
            define(num_items, shape(ratings_column, 1)),

            define(conf_row, alpha * ratings_row),
            define(conf_column, alpha * ratings_column),

            define(conf_u, constant_d(0.0, list(total_num_items))),
            define(conf_i, constant_d(0.0, list(total_num_users))),

            define(c_u, constant_d(0.0, list(total_num_items, total_num_items), nil, nil, nil, "column")),
            define(c_i, constant_d(0.0, list(total_num_users, total_num_users), nil, nil, nil, "column")),

            define(p_u, constant_d(0.0, list(total_num_items))),
            define(p_i, constant_d(0.0, list(total_num_users))),

            set_seed(0),
            define(X_local, random_d(list(total_num_users, num_factors), nil, nil, nil, "column")),
            define(Y_local, random_d(list(total_num_items, num_factors), nil, nil, nil, "column")),

            define(X, all_gather_d(X_local)),
            define(Y, all_gather_d(Y_local)),

            define(I_f, identity_d(num_factors, nil, nil, nil, "row")),
            define(I_i, identity_d(total_num_items, nil, nil, nil, "column")),
            define(I_u, identity_d(total_num_users, nil, nil, nil, "column")),

            define(k, 0),
            define(i, 0),
            define(u, 0),

            define(XtX, dot_d(transpose_d(X_local), X_local) + regularization * I_f),
            define(YtY, dot_d(transpose_d(Y_local), Y_local) + regularization * I_f),

            define(A, constant_d(0.0, list(num_factors, num_factors))),
            define(b, constant_d(0.0, list(num_factors))),

            define(b_u, constant_d(0.0, list(num_factors, total_num_items), nil, nil, nil, "row")),
            define(b_i, constant_d(0.0, list(num_factors, total_num_users), nil, nil, nil, "row")),


            while(k < iterations,
                block(
                    if(enable_output,
                            block(
                                    cout("iteration ", k),
                                    cout("X: ", A),
                                    cout("Y: ", X_local)
                            )
                    ),

                    while(u < total_num_users,
                        block(
                            store(conf_u, slice_row(conf_column, u)),
                            store(c_u, diag_d(conf_u, 0, "column")),
                            store(p_u, __ne(conf_u, 0.0, true)),
                            store(A, dot_d(dot_d(transpose(Y_local), c_u), Y_local) + YtY),

                            store(b_u, dot_d(transpose_d(Y_local), (c_u + I_i))),

                            //store(b, dot_d(b_u, p_u)),

                            //store(slice_row(X_local, u), dot_d(inverse_d(A), b)),

                            //store(slice(X_local, list(u, u + 1, 1), nil), dot(inverse(A), b)),
                            store(u, u + 1)
                        )
                    ),
                    store(u, 0),

                    store(XtX, dot_d(transpose_d(X_local), X_local) + regularization * I_f),

                    //while(i < total_num_items,
                    //    block(
                    //        store(conf_i, slice_column(conf_row, i)),
                    //        store(c_i, diag_d(conf_i, 0, "column")),
                    //        store(p_i, __ne(conf_i, 0.0, true)),
                    //        store(A, dot_d(dot_d(transpose(X_local), c_i), X_local) + XtX),
                    //        store(b_i, dot_d(transpose_d(X_local), (c_i + I_u))),
                    //        store(b, dot_d(b_i, p_i)),
                    //        store(slice_column(Y_local, i), dot_d(inverse_d(A), b)),
                    //        store(i, i + 1)
                    //    )
                    //),
                    //store(i, 0),
                    //store(YtY, dot_d(transpose_d(Y_local), Y_local) + regularization * I_f),

                    store(k, k + 1)
                )
            ),
            list(X_local, b_u)
        )
    )
    __als
)";

////////////////////////////////////////////////////////////////////////////////

std::tuple<std::int64_t, std::int64_t> calculate_tiling_parameters(std::int64_t start,
    std::int64_t stop)
{
    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::uint32_t this_locality = hpx::get_locality_id();

    std::int64_t dim = stop - start;

    if (dim > num_localities)
    {
        std::size_t size = static_cast<std::size_t>(dim / num_localities);
        std::size_t remainder = dim % num_localities;

        if (this_locality < remainder)
        {
            size++;
        }

        if (remainder != 0 && this_locality >= remainder)
        {
            start =
                (size + 1) * remainder + size * (this_locality - remainder);
        }
        else
        {
            start = size * this_locality;
        }

        stop = start + size;
    }
    return std::make_tuple(start, stop);
}

////////////////////////////////////////////////////////////////////////////////
int hpx_main(hpx::program_options::variables_map& vm)
{
    if (vm.count("data_csv") == 0)
    {
        std::cerr << "Please specify '--data_csv=data-file'";
        return hpx::finalize();
    }

    // compile the given code
    using namespace phylanx::execution_tree;

    compiler::function_list snippets;
    auto const& code_read_r = compile("read_r", read_r_code, snippets);
    auto read_r = code_read_r.run();

    // handle command line arguments
    auto filename = vm["data_csv"].as<std::string>();

    auto row_start = vm["row_start"].as<std::int64_t>();
    auto row_stop = vm["row_stop"].as<std::int64_t>();
    auto col_start = vm["col_start"].as<std::int64_t>();
    auto col_stop = vm["col_stop"].as<std::int64_t>();

    auto alpha = vm["alpha"].as<double>();
    auto regularization = vm["regularization"].as<double>();
    auto num_factors = vm["factors"].as<int64_t>();
    auto iterations = vm["num_iterations"].as<std::int64_t>();
    //bool enable_output = vm.count("enable_output") != 0;
    bool enable_output = 1;

    // calculate tiling parameters for this locality, read data
    primitive_argument_type ratings_row, ratings_column;
    std::int64_t user_row_start, user_row_stop, item_col_start, item_col_stop;

    std::tie(user_row_start, user_row_stop) = calculate_tiling_parameters(row_start, row_stop);
    ratings_row = read_r(filename, user_row_start, user_row_stop, col_start, col_stop);

    std::tie(item_col_start, item_col_stop) = calculate_tiling_parameters(col_start, col_stop);
    ratings_column = read_r(filename, row_start, row_stop, item_col_start, item_col_stop);


    std::uint32_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::cout << " total num_localities: " << num_localities << std::endl;

    if (hpx::get_locality_id() == 0)
    {
        std::cout << " on loc 0: \n"
                  << "there are " << num_localities << " localities \n"
                  << "user row partition: \n"
                  << " user_row_start: " << user_row_start
                  << " user_row_stop: " << user_row_stop
                  << " col_start: " << col_start
                  << " col_stop: " << col_stop 
                  << " item column partition: \n"
                  << " row_start: " << row_start
                  << " row_stop: " << row_stop
                  << " item_col_start: " << item_col_start
                  << " item_col_stop: " << item_col_stop
                  << std::endl;
    }
    if (hpx::get_locality_id() == 1)
    {
        std::cout << " on loc 1: \n"
                  << "there are " << num_localities << " localities "
                  << "user row partition: \n"
                  << " user_row_start: " << user_row_start
                  << " user_row_stop: " << user_row_stop
                  << " col_start: " << col_start
                  << " col_stop: " << col_stop 
                  << " item column partition: \n"
                  << " row_start: " << row_start
                  << " row_stop: " << row_stop
                  << " item_col_start: " << item_col_start
                  << " item_col_stop: " << item_col_stop
                  << std::endl;
    }

    // evaluate ALS using the read data
    auto const& code_als = compile("als", als_code, snippets);
    auto als = code_als.run();

    // time the execution
    hpx::evaluate_active_counters(true, "start");
    hpx::util::high_resolution_timer t;

    auto result =
        als(std::move(ratings_row), std::move(ratings_column), regularization,
            num_factors, iterations, alpha, enable_output);

    auto time_diff = t.elapsed();
    std::cout << "Calculated in: " << time_diff << " seconds" << std::endl;

    hpx::evaluate_active_counters(true, " finish");

    auto result_r = extract_list_value(result);
    auto it = result_r.begin();

    //std::cout << "X: \n"
    //          << extract_numeric_value(*it++)
    //          << "\nY: \n"
    //          << extract_numeric_value(*it)
    //          << std::endl;
    //std::cout << "Calculated in: " << time_diff << " seconds" << std::endl;

    if (hpx::get_locality_id() == 0)
    {
        auto result_r = extract_list_value(result);
        auto it_0 = result_r.begin();
        std::cout << " on loc 0: \n"
                  << "there are " << num_localities << " localities \n"
                  << "X: "
                  << extract_numeric_value(*it_0++)
                  << "\nY: "
                  << extract_numeric_value(*it_0)
                  << std::endl;
    }
    if (hpx::get_locality_id() == 1)
    {

        auto result_r_1 = extract_list_value(result);
        auto it_1 = result_r_1.begin();
        std::cout << " on loc 1: \n"
                  << "there are " << num_localities << " localities \n"
                  << "X: "
                  << extract_numeric_value(*it_1++)
                  << "\nY: "
                  << extract_numeric_value(*it_1)
                  << std::endl;
    }

    return hpx::finalize();
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    using hpx::program_options::options_description;
    using hpx::program_options::value;

    // command line handling
    options_description desc("usage: als [options]");
    desc.add_options()
        ("enable_output,e",
          "enable progress output (default: false)")
        ("num_iterations,n", value<std::int64_t>()->default_value(3),
          "number of iterations (default: 10.0)")
        ("factors,f", value<std::int64_t>()->default_value(5),
         "number of factors (default: 10)")
        ("alpha,a", value<double>()->default_value(40),
          "alpha (default: 40)")
        ("regularization,r", value<double>()->default_value(0.1),
         "regularization (default: 0.1)")
        ("data_csv", value<std::string>(), "file name for reading data")
        ("row_start", value<std::int64_t>()->default_value(0),
          "row_start (default: 0)")
        ("row_stop", value<std::int64_t>()->default_value(8),
          "row_stop (default: 569)")
        ("col_start", value<std::int64_t>()->default_value(0),
          "col_start (default: 0)")
        ("col_stop", value<std::int64_t>()->default_value(20),
          "col_stop (default: 30)")
    ;

    // make sure hpx_main is run on all localities
    std::vector<std::string> cfg = {
        "hpx.run_hpx_main!=1"
    };

    return hpx::init(desc, argc, argv, cfg);
}