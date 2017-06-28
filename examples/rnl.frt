\PushVar:rnl@start:1
\DefCmd:rnl@IgnoreNls:0{\IfEq:{\Eof}:0{\IfEq:{\PeekToken}{
}{\Ignore:{\NextToken}\rnl@IgnoreNls}}}
\After{\DefOp{
}{\IfEq:{\rnl@start}:1{\rnl@IgnoreNls\SetVar:rnl@start:0}\Else{\IfEq:{\Eof}:0{\IfEq:{\PeekToken}{
}{\Ignore:{\NextToken}`
`
}\Else{ }}\rnl@IgnoreNls}}\EvalAgain}

\SetCounter:sec@sec:0
\SetCounter:sec@subsec:0
\DefCmd:Section:1{
\Increase:sec@sec=== \sec@sec \1 ===``
``
`\SetVar:rnl@start:1\SetCounter:sec@subsec:0}
\DefCmd:SubSection:1{
\Increase:sec@subsec== \sec@sec.\sec@subsec \1 ==``
`\SetVar:rnl@start:1}
