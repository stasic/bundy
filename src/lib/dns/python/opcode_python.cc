// Copyright (C) 2010  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <Python.h>

#include <dns/opcode.h>
#include <util/python/pycppwrapper_util.h>

#include "pydnspp_common.h"
#include "opcode_python.h"
#include "edns_python.h"

using namespace bundy::dns;
using namespace bundy::dns::python;
using namespace bundy::util;
using namespace bundy::util::python;

namespace {

class s_Opcode : public PyObject {
public:
    s_Opcode() : cppobj(NULL), static_code(false) {}
    const bundy::dns::Opcode* cppobj;
    bool static_code;
};

typedef CPPPyObjectContainer<s_Opcode, Opcode> OpcodeContainer;

int Opcode_init(s_Opcode* const self, PyObject* args);
void Opcode_destroy(s_Opcode* const self);

PyObject* Opcode_getCode(const s_Opcode* const self);
PyObject* Opcode_toText(const s_Opcode* const self);
PyObject* Opcode_str(PyObject* self);

PyMethodDef Opcode_methods[] = {
    { "get_code", reinterpret_cast<PyCFunction>(Opcode_getCode), METH_NOARGS,
      "Returns the code value" },
    { "to_text", reinterpret_cast<PyCFunction>(Opcode_toText), METH_NOARGS,
      "Returns the text representation" },
    { NULL, NULL, 0, NULL }
};


int
Opcode_init(s_Opcode* const self, PyObject* args) {
    uint8_t code = 0;
    if (PyArg_ParseTuple(args, "b", &code)) {
        try {
            self->cppobj = new Opcode(code);
            self->static_code = false;
        } catch (const bundy::OutOfRange& ex) {
            PyErr_SetString(PyExc_OverflowError, ex.what());
            return (-1);
        } catch (...) {
            PyErr_SetString(po_IscException, "Unexpected exception");
            return (-1);
        }
        return (0);
    }

    PyErr_Clear();
    PyErr_SetString(PyExc_TypeError, "Invalid arguments to Opcode constructor");

    return (-1);
}

void
Opcode_destroy(s_Opcode* const self) {
    // Depending on whether we created the rcode or are referring
    // to a global static one, we do or do not delete self->cppobj here
    if (!self->static_code) {
        delete self->cppobj;
    }
    self->cppobj = NULL;
    Py_TYPE(self)->tp_free(self);
}

PyObject*
Opcode_getCode(const s_Opcode* const self) {
    return (Py_BuildValue("I", self->cppobj->getCode()));
}

PyObject*
Opcode_toText(const s_Opcode* const self) {
    return (Py_BuildValue("s", self->cppobj->toText().c_str()));
}

PyObject*
Opcode_str(PyObject* self) {
    // Simply call the to_text method we already defined
    return (PyObject_CallMethod(self,
                                const_cast<char*>("to_text"),
                                const_cast<char*>("")));
}

PyObject*
Opcode_richcmp(const s_Opcode* const self, const s_Opcode* const other,
               const int op)
{
    bool c = false;

    // Check for null and if the types match. If different type,
    // simply return False
    if (!other || (self->ob_type != other->ob_type)) {
        Py_RETURN_FALSE;
    }

    // Only equals and not equals here, unorderable type
    switch (op) {
    case Py_LT:
        PyErr_SetString(PyExc_TypeError, "Unorderable type; Opcode");
        return (NULL);
    case Py_LE:
        PyErr_SetString(PyExc_TypeError, "Unorderable type; Opcode");
        return (NULL);
    case Py_EQ:
        c = (*self->cppobj == *other->cppobj);
        break;
    case Py_NE:
        c = (*self->cppobj != *other->cppobj);
        break;
    case Py_GT:
        PyErr_SetString(PyExc_TypeError, "Unorderable type; Opcode");
        return (NULL);
    case Py_GE:
        PyErr_SetString(PyExc_TypeError, "Unorderable type; Opcode");
        return (NULL);
    }
    if (c)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

} // end of unnamed namespace

namespace bundy {
namespace dns {
namespace python {

PyTypeObject opcode_type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pydnspp.Opcode",
    sizeof(s_Opcode),                   // tp_basicsize
    0,                                  // tp_itemsize
    (destructor)Opcode_destroy,         // tp_dealloc
    NULL,                               // tp_print
    NULL,                               // tp_getattr
    NULL,                               // tp_setattr
    NULL,                               // tp_reserved
    NULL,                               // tp_repr
    NULL,                               // tp_as_number
    NULL,                               // tp_as_sequence
    NULL,                               // tp_as_mapping
    NULL,                               // tp_hash
    NULL,                               // tp_call
    Opcode_str,                         // tp_str
    NULL,                               // tp_getattro
    NULL,                               // tp_setattro
    NULL,                               // tp_as_buffer
    Py_TPFLAGS_DEFAULT,                 // tp_flags
    "The Opcode class objects represent standard OPCODEs "
    "of the header section of DNS messages.",
    NULL,                               // tp_traverse
    NULL,                               // tp_clear
    (richcmpfunc)Opcode_richcmp,        // tp_richcompare
    0,                                  // tp_weaklistoffset
    NULL,                               // tp_iter
    NULL,                               // tp_iternext
    Opcode_methods,                     // tp_methods
    NULL,                               // tp_members
    NULL,                               // tp_getset
    NULL,                               // tp_base
    NULL,                               // tp_dict
    NULL,                               // tp_descr_get
    NULL,                               // tp_descr_set
    0,                                  // tp_dictoffset
    (initproc)Opcode_init,              // tp_init
    NULL,                               // tp_alloc
    PyType_GenericNew,                  // tp_new
    NULL,                               // tp_free
    NULL,                               // tp_is_gc
    NULL,                               // tp_bases
    NULL,                               // tp_mro
    NULL,                               // tp_cache
    NULL,                               // tp_subclasses
    NULL,                               // tp_weaklist
    NULL,                               // tp_del
    0,                                  // tp_version_tag
    BUNDY_UTIL_PYTHON_PyVarObject_TAIL_INIT
};

PyObject*
createOpcodeObject(const Opcode& source) {
    OpcodeContainer container(PyObject_New(s_Opcode, &opcode_type));
    container.set(new Opcode(source));
    return (container.release());
}

bool
PyOpcode_Check(PyObject* obj) {
    if (obj == NULL) {
        bundy_throw(PyCPPWrapperException, "obj argument NULL in typecheck");
    }
    return (PyObject_TypeCheck(obj, &opcode_type));
}

const Opcode&
PyOpcode_ToOpcode(const PyObject* opcode_obj) {
    if (opcode_obj == NULL) {
        bundy_throw(PyCPPWrapperException,
                  "obj argument NULL in Opcode PyObject conversion");
    }
    const s_Opcode* opcode = static_cast<const s_Opcode*>(opcode_obj);
    return (*opcode->cppobj);
}

} // end python namespace
} // end dns namespace
} // end bundy namespace
