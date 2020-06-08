// Copyright (c) 2019 Bita Hasheminezhad
// Copyright (c) 2019 Hartmut Kaiser
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <phylanx/phylanx.hpp>

#include <hpx/hpx_main.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/modules/testing.hpp>

#include <string>
#include <utility>

phylanx::execution_tree::primitive_argument_type compile_and_run(
    std::string const& codestr)
{
    phylanx::execution_tree::compiler::function_list snippets;
    phylanx::execution_tree::compiler::environment env =
        phylanx::execution_tree::compiler::default_environment();

    auto const& code = phylanx::execution_tree::compile(codestr, snippets, env);
    return code.run().arg_;
}

///////////////////////////////////////////////////////////////////////////////
void test_conv2d_trans_operation(std::string const& code,
    std::string const& expected_str)
{
    HPX_TEST_EQ(compile_and_run(code), compile_and_run(expected_str));
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    test_conv2d_trans_operation(
        R"(conv2d_transpose([[[[1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]],
                             [[-1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]]],
                             [[[1., 2., 0., 4., -2.], [0., 1., 5., 4., 5.]],
                             [[-1., 3., 3., -6., -1.],[-3., 3., 0., 2., 0.]]]],
                 [[[[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]],
                 [[ 41,  42,  43,  44,  45], [ 46,  47,  48,  49,  50],
                  [ 51,  52,  53,  54,  55], [ 56,  57,  58,  59,  60]]],
                [[[ 6,  2,  0,  4,  5], [ 6,  7,  1,  9,  0],
                  [ 7,  2,  3,  4,  5], [ 6,  7,  8,  9,  0]],
                 [[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]]]],
                 list(2,3,4,4)))",
             R"([[[[  1.,   6.,  11.,  16.], [ 21.,  26.,  31.,  36.],
                   [ 41.,  46.,  51.,  56.], [  0.,   0.,   0.,   0.]],
                  [[  5.,   0.,  -4., -10.], [-20., -20., -20., -20.],
                   [-20., -20., -20., -20.], [  0.,   0.,   0.,   0.]],
                  [[ -6.,  -6.,  -7.,  -6.], [ -1.,  -6., -11., -16.],
                   [-21., -26., -31., -36.], [  0.,   0.,   0.,   0.]]],
                 [[[ 11.,  36.,  61.,  86.], [169., 269., 369., 469.],
                   [569., 669., 769., 869.], [658., 733., 808., 883.]],
                  [[  1.,  31., -18.,  11.], [ 10.,  40.,  75., 125.],
                   [125., 225., 325., 425.], [449., 534., 619., 704.]],
                  [[-29., -36., -21., -15.], [-19.,  -4., -42., -24.],
                   [-44., -44., -44., -44.], [ 51.,  61.,  71.,  81.]]]])");
    test_conv2d_trans_operation(
        R"(conv2d_transpose([[[[1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]],
                             [[-1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]]],
                             [[[1., 2., 0., 4., -2.], [0., 1., 5., 4., 5.]],
                             [[-1., 3., 3., -6., -1.],[-3., 3., 0., 2., 0.]]]],
                 [[[[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]],
                 [[ 41,  42,  43,  44,  45], [ 46,  47,  48,  49,  50],
                  [ 51,  52,  53,  54,  55], [ 56,  57,  58,  59,  60]]],
                [[[ 6,  2,  0,  4,  5], [ 6,  7,  1,  9,  0],
                  [ 7,  2,  3,  4,  5], [ 6,  7,  8,  9,  0]],
                 [[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]]]],
                 list(2,2,2,4), "same"))",
             R"([[[[ 21.,  26.,  31.,  36.], [ 41.,  46.,  51.,  56.]],
                [[-20., -20., -20., -20.], [-20., -20., -20., -20.]]],
                [[[169., 269., 369., 469.], [569., 669., 769., 869.]],
                [[ 10.,  40.,  75., 125.], [125., 225., 325., 425.]]]])");
    test_conv2d_trans_operation(
        R"(conv2d_transpose([[[[1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]],
                             [[-1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]]],
                             [[[1., 2., 0., 4., -2.], [0., 1., 5., 4., 5.]],
                             [[-1., 3., 3., -6., -1.],[-3., 3., 0., 2., 0.]]]],
                 [[[[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]],
                 [[ 41,  42,  43,  44,  45], [ 46,  47,  48,  49,  50],
                  [ 51,  52,  53,  54,  55], [ 56,  57,  58,  59,  60]]],
                [[[ 6,  2,  0,  4,  5], [ 6,  7,  1,  9,  0],
                  [ 7,  2,  3,  4,  5], [ 6,  7,  8,  9,  0]],
                 [[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]]]],
                 list(2,4,5,4), "valid", list(2,2)))",
        R"([[[[  1.,   6.,  11.,  16.], [ 21.,  26.,  31.,  36.],
                 [ 41.,  46.,  51.,  56.], [  0.,   0.,   0.,   0.],
                 [  0.,   0.,   0.,   0.]],[[  6.,   6.,   7.,   6.],
                 [  1.,   6.,  11.,  16.], [ 21.,  26.,  31.,  36.],
                 [  0.,   0.,   0.,   0.], [  0.,   0.,   0.,   0.]],
                [[ -1.,  -6., -11., -16.], [-21., -26., -31., -36.],
                 [-41., -46., -51., -56.], [  0.,   0.,   0.,   0.],
                 [  0.,   0.,   0.,   0.]],[[ -6.,  -6.,  -7.,  -6.],
                 [ -1.,  -6., -11., -16.], [-21., -26., -31., -36.],
                 [  0.,   0.,   0.,   0.], [  0.,   0.,   0.,   0.]]],
               [[[ 11.,  36.,  61.,  86.], [111., 136., 161., 186.],
                 [269., 369., 469., 569.], [358., 433., 508., 583.],
                 [658., 733., 808., 883.]],[[ 16.,  56.,  17.,  56.],
                 [ 11.,  36.,  61.,  86.], [154., 184., 219., 269.],
                 [ 58., 133., 208., 283.], [358., 433., 508., 583.]],
                [[-15., -25., -35., -45.], [-55., -65., -75., -85.],
                 [-84., -84., -84., -84.], [ 51.,  61.,  71.,  81.],
                 [ 91., 101., 111., 121.]],[[-29., -36., -21., -15.],
                 [-15., -25., -35., -45.], [-59., -44., -82., -64.],
                 [ 11.,  21.,  31.,  41.], [ 51.,  61.,  71.,  81.]]]])");
    test_conv2d_trans_operation(
        R"(conv2d_transpose([[[[1., 0., 0., 0., 0.], [2., 0., 0., 2., 0.]],
                             [[-1., 0., 0., 0., 0.], [0., 0., 0., 3., 0.]]]],
                 [[[[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]],
                 [[ 41,  42,  43,  44,  45], [ 46,  47,  48,  49,  50],
                  [ 51,  52,  53,  54,  55], [ 56,  57,  58,  59,  60]]],
                [[[ 6,  2,  0,  4,  5], [ 6,  7,  1,  9,  0],
                  [ 7,  2,  3,  4,  5], [ 6,  7,  8,  9,  0]],
                 [[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]]]],
                 list(1, 4, 6, 4), "valid", list(2, 3)))",
        R"([[[[  1.,   6.,  11.,  16.],[ 21.,  26.,  31.,  36.],
              [ 41.,  46.,  51.,  56.],[ 10.,  30.,  50.,  70.],
              [ 90., 110., 130., 150.],[170., 190., 210., 230.]],
             [[  6.,   6.,   7.,   6.],[  1.,   6.,  11.,  16.],
              [ 21.,  26.,  31.,  36.],[ 20.,  30.,  22.,  30.],
              [ 10.,  30.,  50.,  70.],[ 90., 110., 130., 150.]],
             [[ -1.,  -6., -11., -16.],[-21., -26., -31., -36.],
              [-41., -46., -51., -56.],[ 12.,  27.,  42.,  57.],
              [ 72.,  87., 102., 117.],[132., 147., 162., 177.]],
             [[ -6.,  -6.,  -7.,  -6.],[ -1.,  -6., -11., -16.],
              [-21., -26., -31., -36.],[ 12.,  27.,  12.,  27.],
              [ 12.,  27.,  42.,  57.],[ 72.,  87., 102., 117.]]]])");
    test_conv2d_trans_operation(
        R"(conv2d_transpose([[[[1., 0., 0., 0., 0.],[0., 0., 0., 0., 0.]],
                            [[-1., 0., 0., 0., 0.],[0., 0., 0., 4., 0.]]],
                            [[[1., 2., 0., 4., -2.],[0., 1., 5., 4., 5.]],
                           [[-1., 3., 3., -6., -1.],[-3., 3., 0., 2., 0.]]]],
                 [[[[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]],
                 [[ 41,  42,  43,  44,  45], [ 46,  47,  48,  49,  50],
                  [ 51,  52,  53,  54,  55], [ 56,  57,  58,  59,  60]]],
                [[[ 6,  2,  0,  4,  5], [ 6,  7,  1,  9,  0],
                  [ 7,  2,  3,  4,  5], [ 6,  7,  8,  9,  0]],
                 [[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
                  [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
                 [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
                  [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]]]],
                 list(2, 4, 5, 4), "same", list(2, 3)))",
        R"([[[[   1.,    6.,   11.,   16.], [  21.,   26.,   31.,   36.],
             [  41.,   46.,   51.,   56.], [   0.,    0.,    0.,    0.],
             [   0.,    0.,    0.,    0.]],[[   6.,    6.,    7.,    6.],
             [   1.,    6.,   11.,   16.], [  21.,   26.,   31.,   36.],
             [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.]],
            [[  -1.,   -6.,  -11.,  -16.], [ -21.,  -26.,  -31.,  -36.],
             [ -41.,  -46.,  -51.,  -56.], [  16.,   36.,   56.,   76.],
             [  96.,  116.,  136.,  156.]],[[  -6.,   -6.,   -7.,   -6.],
             [  -1.,   -6.,  -11.,  -16.], [ -21.,  -26.,  -31.,  -36.],
             [  16.,   36.,   16.,   36.], [  16.,   36.,   56.,   76.]]],
           [[[  11.,   36.,   61.,   86.], [ 111.,  136.,  161.,  186.],
             [ 211.,  236.,  261.,  286.], [  58.,  133.,  208.,  283.],
             [ 358.,  433.,  508.,  583.]],[[  16.,   56.,   17.,   56.],
             [  11.,   36.,   61.,   86.], [ 111.,  136.,  161.,  186.],
             [  43.,   48.,   58.,   83.], [  58.,  133.,  208.,  283.]],
            [[ -15.,  -25.,  -35.,  -45.], [ -55.,  -65.,  -75.,  -85.],
             [ -95., -105., -115., -125.], [  11.,   21.,   31.,   41.],
             [  51.,   61.,   71.,   81.]],[[ -29.,  -36.,  -21.,  -15.],
             [ -15.,  -25.,  -35.,  -45.], [ -55.,  -65.,  -75.,  -85.],
             [  -4.,   21.,   -7.,   21.], [  11.,   21.,   31.,   41.]]]])");
    test_conv2d_trans_operation(
        R"(conv2d_transpose([[[[1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]],
                            [[-1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]]],
                            [[[1., 2., 0., 4., -2.], [0., 1., 5., 4., 5.]],
                          [[-1., 3., 3., -6., -1.], [-3., 3., 0., 2., 0.]]]],
       [[[[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
         [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
        [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
         [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]],
        [[ 41,  42,  43,  44,  45], [ 46,  47,  48,  49,  50],
         [ 51,  52,  53,  54,  55], [ 56,  57,  58,  59,  60]]],
       [[[ 6,  2,  0,  4,  5], [ 6,  7,  1,  9,  0],
         [ 7,  2,  3,  4,  5], [ 6,  7,  8,  9,  0]],
        [[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
         [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
        [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
         [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]]]],
        list(2, 2, 2, 4), "same", list(1,1), list(2, 2)))",
        R"([[[[-21., -26., -31., -36.], [  0.,   0.,   0.,   0.]],
        [[  1.,   6.,  11.,  16.], [  0.,   0.,   0.,   0.]]],
       [[[-55., -65., -75., -85.], [ 51.,  61.,  71.,  81.]],
        [[ 11.,  36.,  61.,  86.], [ 58., 133., 208., 283.]]]])");
    test_conv2d_trans_operation(
        R"(conv2d_transpose([[[[1., 0., 0., 0., 0.], [0., 0., 0., 0., 0.]],
                            [[-1., 0., 0., 10., 0.], [0., 0., 0., 0., 0.]]],
                            [[[1., 2., 0., 4., -2.], [0., 1., 5., 4., 5.]],
                            [[-1., 3., 3., -6., -1.], [-3., 3., 0., 2., 0.]]]],
       [[[[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
         [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
        [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
         [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]],
        [[ 41,  42,  43,  44,  45], [ 46,  47,  48,  49,  50],
         [ 51,  52,  53,  54,  55], [ 56,  57,  58,  59,  60]]],
       [[[ 6,  2,  0,  4,  5], [ 6,  7,  1,  9,  0],
         [ 7,  2,  3,  4,  5], [ 6,  7,  8,  9,  0]],
        [[  1,   2,   3,   4,   5], [  6,   7,   8,   9,  10],
         [ 11,  12,  13,  14,  15], [ 16,  17,  18,  19,  20]],
        [[ 21,  22,  23,  24,  25], [ 26,  27,  28,  29,  30],
         [ 31,  32,  33,  34,  35], [ 36,  37,  38,  39,  40]]]],
        list(2, 5, 8, 4), "valid", list(1,1), list(3, 3)))",
        R"([[[[   1.,    6.,   11.,   16.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [  21.,   26.,   31.,   36.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [  41.,   46.,   51.,   56.], [   0.,    0.,    0.,    0.]],
        [[  39.,   84.,  129.,  174.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [ 219.,  264.,  309.,  354.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [ 399.,  444.,  489.,  534.], [   0.,    0.,    0.,    0.]],
        [[   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.]],
        [[   6.,    6.,    7.,    6.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [   1.,    6.,   11.,   16.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [  21.,   26.,   31.,   36.], [   0.,    0.,    0.,    0.]],
        [[  34.,   84.,   33.,   84.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [  39.,   84.,  129.,  174.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [ 219.,  264.,  309.,  354.], [   0.,    0.,    0.,    0.]]],
       [[[  11.,   36.,   61.,   86.], [  58.,  133.,  208.,  283.],
         [   0.,    0.,    0.,    0.], [ 111.,  136.,  161.,  186.],
         [ 358.,  433.,  508.,  583.], [   0.,    0.,    0.,    0.],
         [ 211.,  236.,  261.,  286.], [ 658.,  733.,  808.,  883.]],
        [[ -15.,  -25.,  -35.,  -45.], [  11.,   21.,   31.,   41.],
         [   0.,    0.,    0.,    0.], [ -55.,  -65.,  -75.,  -85.],
         [  51.,   61.,   71.,   81.], [   0.,    0.,    0.,    0.],
         [ -95., -105., -115., -125.], [  91.,  101.,  111.,  121.]],
        [[   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.],
         [   0.,    0.,    0.,    0.], [   0.,    0.,    0.,    0.]],
        [[  16.,   56.,   17.,   56.], [  43.,   48.,   58.,   83.],
         [   0.,    0.,    0.,    0.], [  11.,   36.,   61.,   86.],
         [  58.,  133.,  208.,  283.], [   0.,    0.,    0.,    0.],
         [ 111.,  136.,  161.,  186.], [ 358.,  433.,  508.,  583.]],
        [[ -29.,  -36.,  -21.,  -15.], [  -4.,   21.,   -7.,   21.],
         [   0.,    0.,    0.,    0.], [ -15.,  -25.,  -35.,  -45.],
         [  11.,   21.,   31.,   41.], [   0.,    0.,    0.,    0.],
         [ -55.,  -65.,  -75.,  -85.], [  51.,   61.,   71.,   81.]]]])");

    return hpx::util::report_errors();
}
