%modules = ( # path to module name map
    "QuickenPerf" => "$basedir/src/perf",
);
%moduleheaders = ( # restrict the module headers to those found in relative path
);

%classnames = (
    #add classnames that are not automatically detected
    #e.g. "someheader.h" => "SomeType"
    "quickenperfmodule.h" => "QuickenPerfModule"
);

# Module dependencies.
# Every module that is required to build this module should have one entry.
# Each of the module version specifiers can take one of the following values:
#   - A specific Git revision.
#   - any git symbolic ref resolvable from the module's repository (e.g. "refs/heads/master" to track master branch)
#
%dependencies = (
    "qtbase" => "",
    "qtdeclarative" => "",
);

$publicclassregexp = "^Quicken[A-Za-z0-9]*::[^:]*\$"
