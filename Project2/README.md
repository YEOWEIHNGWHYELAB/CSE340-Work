# Project 2

## Project Tips

For task 1 you just need to print the first abstract syntax tree for first line. However, that does not mean you don't need build the abstract syntax tree for subsequent statments. Because you will still need to return whether which of the statements has syntax error.

You can print your tree using BFS from top to bottom and from left to right.

If you are implementing task 1, please just read implementation document from page 1 to 11.

Terminal is a symbol you read from the input, and when you reduce it, it becomes an expression. Suppose "A" - obtained from lexer.peek(1), is a terminal. Then you can put it inside stack. "A" is terminal because it is an instance of an ID. And during next iteration of the loop, you will reduce it and it will become an expression. Say if we have E -> ID, ID of "A" gets reduced to an expression.

Whatever your lexer.peek(1) reads, you call it a terminal. Then you will do parsing by parsing SCALAR and then ARRAY and then do the parsing of statement.

In parse_expr(), you will implement operator precedence parsing, where you read 2 symbol a and b, a is obtained from reading the stack while b is obtained from reading the input. The token read from b is always a terminal, and then you reduced it from the grammar rule and pushed to the stack, it will be an expression.

In parse_expr(), you should also separate a function for terminal peeking and parsing so you don't make it too complicated.

Step 2 of parse_expr() is where you do the operator precedence parsing.

You can refer to lecture notes for operator precedence parsing. At step 2 and 3 of parse_expr().

After the step 2 and 3 of parse_expr(), you will have the regex stack of the exprNode. Then it is where you already have abstract syntax tree, and then when the parse_expr() returns, it will return the root exprNode to which you will be performing the BFS on.

Stack node vs Expression node. Stack node are the nodes you are pushing to the stack, it could be a terminal or an expression. But an expression node is the tree you are building and each of which will be a expression.

parse_block() has 2 statements -> assignments and output statements. Block gives assignment and output statements.

You need to have a parse_statement_list() too. Statement list have a statement or statements and statement then have assignment and output list.

In parse_statement_list(), you will then call parse_statement -> then based on that decide whether you want to call parse_outout or parse_assignment statement.

Also the parse_assign_stmt() given in the project implementation document (or your current implementation) will not let you parse the entire statement, that one only lets you parse the expression within the assignment statements, so if the statement is like a[expr] = expr;, you will only parse up to the first expr within the [].

Don't use lexer.peek(), use peek_symbol instead.

Also you should do operator_precedence_parsing inside parse_expr() instead of sepearately.

Also your do while loop, your first while condition should use stack.peek instead of your curr_top since curr_top is already removed and we would like to view the next thing in the stack.

Your current reduction if else consdition is wrong, ID_OPER and WHOLE_ARRAY_OPER need to be done separately. ID_OPER and NUM_OPER can be done together though.

WHOLE_ARRAY_OPER should have 2 nodes in total. Unlike the NUM_OPER and ID_OPER which only have a single EXPR node.
