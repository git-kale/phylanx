//  Copyright (c) 2018 Bibek Wagle
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef PHYLANX_HSTACK_OPERATION_JAN13_1200_2018_H
#define PHYLANX_HSTACK_OPERATION_JAN13_1200_2018_H

#include <phylanx/config.hpp>
#include <phylanx/ast/node.hpp>
#include <phylanx/execution_tree/primitives/base_primitive.hpp>
#include <phylanx/ir/node_data.hpp>

#include <hpx/include/components.hpp>

#include <vector>

namespace phylanx { namespace execution_tree { namespace primitives
{
    class hstack_operation
      : public base_primitive
      , public hpx::components::component_base<hstack_operation>
    {
    public:
        static match_pattern_type const match_data;

        hstack_operation() = default;

        PHYLANX_EXPORT hstack_operation(
            std::vector<primitive_argument_type>&& operands);

        PHYLANX_EXPORT hpx::future<primitive_argument_type> eval(
            std::vector<primitive_argument_type> const& params) const override;
    };
}}}


#endif //PHYLANX_HSTACK_OPERATION_JAN13_1200_2018_H