# Copyright (c) 2017-2019 Hartmut Kaiser
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(tests
    319_fold_right_copy_list
    543_compile_twice
    792_hstack_range
    817_compare_nil
    817_cout_nil
    817_lambda_nil
    1198_inaccessible_variable
    1213_timer_problem
    allow_empty_blocks_278
    assign_variable_twice_491
    assign_wrong_variable_245
    cout_noargs_prints_nothing_159
    cout_prints_twice_155
    define_loses_values_177
    empty_hstack_509
    expression_topology_213
    external_functions_337
    list_iter_space_429
    list_of_variables_244
    negative_integral_value_131
    no_integers_313
    out_of_scope_variable_232
    random_d_1139
    shape1d_357
    shape_2nd_argument_406
    slicing_nil_358
    slicing_reverse_step_550
    support_nil_314
    vector_of_variables_244
    replace_names_374
    allow_return_nil_432
   )

set(817_cout_nil_FLAGS DEPENDENCIES HPX::iostreams_component)
set(assign_wrong_variable_245_FLAGS DEPENDENCIES HPX::iostreams_component)
set(cout_noargs_prints_nothing_159_FLAGS DEPENDENCIES HPX::iostreams_component)
set(cout_prints_twice_155_FLAGS DEPENDENCIES HPX::iostreams_component)
set(define_loses_values_177_FLAGS DEPENDENCIES HPX::iostreams_component)
set(out_of_scope_variable_232_FLAGS DEPENDENCIES HPX::iostreams_component)
set(external_functions_337_FLAGS DEPENDENCIES HPX::iostreams_component)

set(random_d_1139_PARAMETERS LOCALITIES 2)
set(1213_timer_problem_PARAMETERS LOCALITIES 2)

foreach(test ${tests})
  set(sources ${test}.cpp)

  source_group("Source Files" FILES ${sources})

  # add executable
  add_phylanx_executable(${test}_test
    SOURCES ${sources}
    ${${test}_FLAGS}
    EXCLUDE_FROM_ALL
    FOLDER "Tests/Regression/ExecutionTree/")

  add_phylanx_regression_test("execution_tree" ${test} ${${test}_PARAMETERS})

  add_phylanx_pseudo_target(tests.regressions.execution_tree_dir.${test})
  add_phylanx_pseudo_dependencies(tests.regressions.execution_tree_dir
    tests.regressions.execution_tree_dir.${test})
  add_phylanx_pseudo_dependencies(tests.regressions.execution_tree_dir.${test}
    ${test}_test_exe)

endforeach()


