#include "Python.h"
#include "gbfp.h"

static PyObject *ErrorObject;

PyObject* MakeLocationList(location *ptLocation, unsigned int iLocationNum) {
    unsigned int i;
    location *ptLocData;
    PyObject *LocationList;
    PyObject *LocationTuple;

    LocationList = PyList_New(0);

    for (i = 0; i < iLocationNum; i++) {
        ptLocData = ptLocation + i;
        
        LocationTuple = PyTuple_New(2);
        PyTuple_SetItem(LocationTuple, 0, PyInt_FromLong((long) ptLocData->lStart));
        PyTuple_SetItem(LocationTuple, 1, PyInt_FromLong((long) ptLocData->lEnd));
        PyList_Append(LocationList, LocationTuple);
    }

    return LocationList;
}

PyObject* MakeQualifierList(qualifier *ptQualifier, unsigned int iQualifierNum) {
    unsigned int i;
    qualifier *ptQualData;
    PyObject *QualifierList;
    PyObject *QualifierTuple;

    QualifierList = PyList_New(0);

    for (i = 0; i < iQualifierNum; i++) {
        ptQualData = ptQualifier + i;

        QualifierTuple = PyTuple_New(2);
        PyTuple_SetItem(QualifierTuple, 0, PyString_FromString(ptQualData->sQualifier));
        PyTuple_SetItem(QualifierTuple, 1, PyString_FromString(ptQualData->sValue));
        PyList_Append(QualifierList, QualifierTuple);
    }

    return QualifierList;
}

PyObject* MakeFeatureDict(feature *ptFeature) {
    PyObject *FeatureDict;

    FeatureDict =  PyDict_New();

    PyDict_SetItemString(FeatureDict, "feature", PyString_FromString((char *) &(ptFeature->sFeature)));
    PyDict_SetItemString(FeatureDict, "direction", PyString_FromStringAndSize((char *) &(ptFeature->cDirection), 1));
    PyDict_SetItemString(FeatureDict, "start", PyInt_FromLong((long) ptFeature->lStart));
    PyDict_SetItemString(FeatureDict, "end", PyInt_FromLong((long) ptFeature->lEnd));
    PyDict_SetItemString(FeatureDict, "number", PyInt_FromLong((long) ptFeature->iNumber));
    PyDict_SetItemString(FeatureDict, "location_num", PyInt_FromLong((long) ptFeature->iLocationNum));
    PyDict_SetItemString(FeatureDict, "qualifier_num", PyInt_FromLong((long) ptFeature->iQualifierNum));
    PyDict_SetItemString(FeatureDict, "location", MakeLocationList(ptFeature->ptLocation, ptFeature->iLocationNum));
    PyDict_SetItemString(FeatureDict, "qualifier", MakeQualifierList(ptFeature->ptQualifier, ptFeature->iQualifierNum));

   return FeatureDict; 
}

PyObject* MakeGBFFDataDict(gbdata *ptGBFFData) {
    int i;

    PyObject *GBFFDataDict;
    PyObject *FeatureList;

    GBFFDataDict =  PyDict_New();

    PyDict_SetItemString(GBFFDataDict, "locus_name", PyString_FromString((char *) &(ptGBFFData->sLocusName)));
    PyDict_SetItemString(GBFFDataDict, "length", PyInt_FromLong((long) ptGBFFData->lLength));
    PyDict_SetItemString(GBFFDataDict, "type", PyString_FromString((char *) &(ptGBFFData->sType)));
    PyDict_SetItemString(GBFFDataDict, "topology", PyString_FromString((char *) &(ptGBFFData->sTopology)));
    PyDict_SetItemString(GBFFDataDict, "division_code", PyString_FromString((char *) &(ptGBFFData->sDivisionCode)));
    PyDict_SetItemString(GBFFDataDict, "date", PyString_FromString((char *) &(ptGBFFData->sDate)));
    PyDict_SetItemString(GBFFDataDict, "feature_num", PyInt_FromLong((long) ptGBFFData->iFeatureNumber));
    PyDict_SetItemString(GBFFDataDict, "sequence", PyString_FromString(ptGBFFData->sSequence));

    FeatureList = PyList_New(0);

    for(i = 0; i < ptGBFFData->iFeatureNumber; i++) {
        PyList_Append(FeatureList, MakeFeatureDict((ptGBFFData->ptFeatures) + i));
    }

    PyDict_SetItemString(GBFFDataDict, "features", FeatureList);

    return GBFFDataDict;
}

static PyObject* parse(PyObject *self, PyObject *args) {
    int i;
    char *psFileName;

    gbdata **pptGBFFData;

    PyObject *GBFFDataList;

    /* Parsing arguments */
    if (!PyArg_ParseTuple(args, "s", &psFileName)) return NULL;
    
    /* Parsing with C function */
    pptGBFFData = parseGBFF(psFileName);

    if (pptGBFFData != NULL) {
        /* Convert datas from C to Python */
        GBFFDataList = PyList_New(0);

        for (i = 0; *(pptGBFFData + i) != NULL; i++) {
            PyList_Append(GBFFDataList, MakeGBFFDataDict(*(pptGBFFData + i)));
        }

        /*
        freeGBData(pptGBFFData);
        */
        
        return GBFFDataList;
    } else {
        PyErr_SetString(PyExc_IOError, "File not found !!!");

        return NULL;
    }
}         
 
static struct PyMethodDef gbfpy_methods[] = { 
    {"parse", parse, METH_VARARGS},
    {NULL, NULL}
};

void initgbfpy(void) {
    PyObject *parser; 
    parser = Py_InitModule("gbfpy", gbfpy_methods);
    ErrorObject = Py_BuildValue("s", "GBFF parser error !!!");
} 