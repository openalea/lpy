/* ---------------------------------------------------------------------------
#
#       L-Py: L-systems in Python
#
#       Copyright 2003-2016 UMR Cirad/Inria/Inra Agap - Virtual Plant Team
#
#       File author(s): F. Boudon (frederic.boudon@cirad.fr)
#
# ---------------------------------------------------------------------------
#
#                      GNU General Public Licence
#
#       This program is free software; you can redistribute it and/or
#       modify it under the terms of the GNU General Public License as
#       published by the Free Software Foundation; either version 2 of
#       the License, or (at your option) any later version.
#
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS For A PARTICULAR PURPOSE. See the
#       GNU General Public License for more details.
#
#       You should have received a copy of the GNU General Public
#       License along with this program; see the file COPYING. If not,
#       write to the Free Software Foundation, Inc., 59
#       Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# ---------------------------------------------------------------------------
*/

#ifndef __LPY_EXECUTION_H__
#define __LPY_EXECUTION_H__

#include "lpy_config.h"
#include <QtCore/QMutex>

LPY_BEGIN_NAMESPACE

/*---------------------------------------------------------------------------*/

class LPY_API PyExecution {

protected:

    PyExecution();
    static PyExecution * __pyexec;

public:

    static PyExecution * get();

    void acquire();
    void release();

    QMutex __mutex;

};


class LPY_API PyExecutionLocker {
public:
    PyExecutionLocker()  { PyExecution::get()->acquire(); }
    ~PyExecutionLocker() { PyExecution::get()->release(); }
};

/*---------------------------------------------------------------------------*/

LPY_END_NAMESPACE

/*---------------------------------------------------------------------------*/

#endif