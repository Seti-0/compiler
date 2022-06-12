#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "visitor.h"
#include "ast.h"

namespace ast {
    void Num::visit(Visitor& visitor) {
        visitor.visit_num(*this);
    }

    void Var::visit(Visitor& visitor) {
        visitor.visit_var(*this);
    }

    void Op::visit(Visitor& visitor) {
        visitor.visit_op(*this);
    }

    void Call::visit(Visitor& visitor) {
        visitor.visit_call(*this);
    }

    void Pro::visit(Visitor& visitor) {
        visitor.visit_pro(*this);
    }

    void Fn::visit(Visitor& visitor) {
        visitor.visit_fn(*this);
    }
}
