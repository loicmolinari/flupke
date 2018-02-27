# Quicken coding style

Quicken follows the Qt conventions, see:
- https://wiki.qt.io/Coding_Conventions
- https://wiki.qt.io/Qt_Coding_Style

On top of it we add the few clarifications listed below.

## Header inclusion

When including Qt or Quicken headers, always prefix with the project name. Use:

```C++
#include <QtCore/QMap>
#include <QuickenMetrics/applicationmonitor.h>
```

not

```C++
#include <QMap>
#include <applicationmonitor.h>
```

For readability reasons and to avoid hidden dependencies, headers should be
included in ordered sections, separated by an empty line, as follows:

```C++
// The project's related header (for instance here for foo.cpp).
#include "foo.h"

// C standard library headers.
#include <sys/types.h>
#include <unistd.h>

// C++ standard library headers.
#include <functional>
#include <hash_map>
#include <vector>

// Qt and Quicken headers. Try to first list QtCore and QtGui headers. Private
// headers come right after public headers of the same project.
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/private/qobject_p.h>
#include <QtGui/QWindow>
#include <QtGui/private/qcolor_p.h>
#include <QuickenMetrics/applicationmonitor.h>

// Other libraries' headers.
#include <gst/gst.h>
#include <boost/call_traits.hpp>

// The project's headers.
#include "bar.h"
#include "baz_p.h"
```

For libraries, the `"foo"` inclusion variant must be used in C++ source files
for the project's headers. The `<Project/foo.h>` and `<Project/private/foo_p.h>`
inclusion variant must be used for the project's headers in private and public
headers, wherever the header is located in the source tree. That ensures the
inclusion of private headers by external projects won't break.
