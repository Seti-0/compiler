module.exports = grammar({
    name: 'via',
    extras: $ => [/[\t\f ]+/],
    rules: {
      // ################
      // # Magic Fields #
      // ################
      //word: $ => $.identifier,
      // ########################
      // # Top Level Statements #
      // ########################
      source_file: $ => seq(
        repeat('\n'),
        optional(seq(
          $._top_level_statement,
          repeat(seq($._separator, $._top_level_statement)),
          repeat('\n'),
        )),
      ),

      _separator: $ => choice(
        ';', 
        repeat1('\n')
      ),

      // For now, there is no difference between a block
      // and top-level statement.
      _top_level_statement: $ => choice(
        $._block_statement
      ),
      // ###############################
      // # Blocks and Block Statements #
      // ###############################
      // Note that expressions aren't block statements.
      _block_statement: $ => choice(
        $.block, // Blocks can be nested.
        $.function_definition, $.function_call, 
        $.if, $.foreach, $.while,
        $.assignment
      ),

      block: $ => seq(
        '{', 
        repeat('\n'), 
        optional(seq(
          $._block_statement, 
          repeat(seq($._separator, $._block_statement)),
          repeat('\n')
        )),
        '}'
      ),
      // ########################
      // # Function Definitions #
      // ########################
      function_definition: $ => seq(
        'def', $.identifier, $.parameters, '->', $.type,
        $.block
      ),

      // This is a bit complicated.
      // 'optional' means 0 or 1
      // 'repeat' means 0 or more.
      // So this matches (), (param) and (param1, param2, param3)
      parameters: $ => seq(
        '(', optional(seq(repeat(seq($._parameter, ',')), $._parameter)), ')'
      ),

      _parameter: $ => seq(
        $.type, $.identifier
      ),
      // #################
      // # Function Call #
      // #################
      // The choice below is so that Vec2(...) and move(...)
      // are both matched. That is, creating an object is also a 
      // function call.
      function_call: $ => seq(
        choice($.identifier, $.type), $.arguments
      ),

      // Similar to parameters, a bit complicated.
      arguments: $ => choice(
        seq('(', ')'),
        seq('(', $._argument, ')'),
        seq('(', repeat1(seq($._argument, ',')), $._argument, ')')
      ),

      _argument: $ => $._expression,
      // ################
      // # Control Flow #
      // ################
      if: $ => seq(
        'if', $._expression, $.block
      ),

      while: $ => seq(
        'while', $._expression, $.block
      ),

      foreach: $ => seq(
        'for', $.identifier, 'in', $._expression, $.block
      ),
      // ##############
      // # Assignment #
      // ##############
      assignment: $ => seq(
        optional(choice('con', 'var')), $.identifier, '=', $._expression
      ),
      // ##########################################
      // # Expressions, Operators, and Precedence #
      // ##########################################
      _expression: $ => choice(
        $._literal,
        $.identifier,
        $.group,
        $.unary_operation,
        $.binary_operation,
        $.function_call
      ),

      group: $ => seq(
        '(', $._expression, ')'
      ),

      unary_operation: $ => prec.left(5, seq(
        'not', $._expression
      )),

      binary_operation: $ => choice(
        prec.left(4, seq($._expression, choice('*', '/'), $._expression)),
        prec.left(3, seq($._expression, choice('+', '-'), $._expression)),
        prec.left(2, seq($._expression, 'and', $._expression)),
        prec.left(1, seq($._expression, 'or', $._expression)),
      ),
      // ############
      // # Literals #
      // ############
      _literal: $ => choice(
        $.boolean,
        $.number,
        $.string,
        $.list
      ),

      boolean: $ => choice('true', 'false'),
      number: $ => /\d+(\.\d+)?/,
      string: $ => /"[^"\n]*"/,
      list: $ => choice(
        seq('[', ']'),
        seq('[', repeat(seq($._expression, ',')), $._expression, ']')
      ),
      // #########################
      // # Identifiers and Types #
      // #########################
      identifier: $ => /[a-z][a-z0-9_]*/,
      type: $ => /[A-Z][a-zA-Z0-9_]*/
    }
  });
  