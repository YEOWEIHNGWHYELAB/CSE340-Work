# Project 1

## To-Do

1) Optimize the Semantic_Error, currently the REG is still construction despite having semantic errors.

2) To implement epsilon detection, put all the nodes reachable without consuming any input to be in the set first, then see if you can find both start and accept node?

## Problems Left

1) Last test case for syntax error general failing

Because you check for semantic error and epsilon error before expect(INPUT_TEXT) and expect(EOF)
