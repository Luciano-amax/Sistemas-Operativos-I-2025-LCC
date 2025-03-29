-module(factorial).
-export([fact/1]).

% Función factorial por patternmatching

fact(0) -> 1;
fact(N) -> N * fact(N-1).