module.exports = grammar({

    name: 'via',
    
    ////////////////////////
    // Ignored Whitespace //
    ////////////////////////

    // Notice that tabs and spaces are included below.
    // But newlines aren't! This language is aware of newlines,
    // they can end statements that are ready to end.

extras: $ => [$.comment, /[\t ]+/],
    ///////////
    // Rules //
    ///////////

    rules: {

      //////////////////////////////////////
      // Start Rule: Top-Level Statements //
      //////////////////////////////////////
      
      // Notice that the optional skipping of newlines is defined at the start here.
      // Merging adjacent rules isn't supported, so it's important not to double-up
      // on these or errors regarding ambiguity occur. The rule of thumb followed is that
      // sub-statements always handle any following whitespace-skipping as needed. This
      // allows dealing with optional cases that would be hard to express otherwise.

      source_file: $ => seq(
        optional($._newline),
        optional($._statements),
      ),

      //////////////
      // Newlines //
      //////////////
     
      // One or more newlines, accounting for the Windows
      // silliness with '\r' characters.
      _newline: $ => repeat1(seq(optional('\r'), '\n')),

      //////////////
      // Comments //
      //////////////

      comment: $ => choice(
        /#.*/,
        seq(
          '/*',
          repeat(/./s),
          '*/'
        )
      ),

//      _block_comment: $ => /\/\*[^*]*\*+([^\/*][^*]*\*+)+\//,

      ///////////////////////////
      // Blocks and Statements //
      ///////////////////////////

      // Note that statements aren't divided into top-level/block
      // in the grammar here. For now, there is actually no difference!

      // But even if there was a difference - some being allowed, some not,
      // it makes to ignore the distinction in the initial grammar, since 
      // we can send back better errors by operating on the concrete syntax
      // tree once it's ready.

      _statements: $ => seq(
        $._statement_line,
        repeat(seq($._newline, $._statement_line)),
        optional($._newline)
      ),

      _statement_line: $ => choice(
        $._statement,
        seq(repeat1($.block), optional($._statement))
      ),

      _statement: $ => choice(
        $.function_call
      ),

      _separator: $ => choice(
        seq(';', optional($._newline)),
        $._newline
      ),

      block: $ => seq(
        '{', 
        optional($._newline),
        optional($._statements),
        '}',
      ),

      scope: $ => seq(
        'scope',
        $.block
      ),

      /////////////////
      // Expressions //
      /////////////////

      _expression: $ => choice(
        $.function_call,
        $._numeric_literal,
        $.name
      ),

      ////////////////////
      // Function Calls //
      ////////////////////

      function_call: $ => seq(
        $._expression, // Should there be a skip here?
        '(', optional($._newline),
        optional(seq(
          $._expression,
          optional($._newline),
          repeat(seq(
            ',', optional($._newline),
            $._expression, optional($._newline)
          ))
        )),
        ')'
      ),

      //////////////
      // Literals //
      //////////////

      _numeric_literal: $ => choice(
        $._decimal_literal,
      ),

      _decimal_literal: $ => /((-?(\d+)(_\d+)*)|((-?(\d+)(_\d+)*)?(\.\d+(_\d+)*)))(e-?\d+(_\d+)*)?/,

      ///////////
      // Names //
      ///////////

      name: $ => /[_a-z]+/,
      type_name: $ => /[_A-Z][_a-zA-Z]*/,

      /*
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
      identifier: $ => /[a-z][a-z0-9_]*//*,
      type: $ => /[A-Z][a-zA-Z0-9_]*/
    }
  });
  