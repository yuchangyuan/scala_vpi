#include <jni.h>
#include "me_ycy_verilog_vpi_Vpi.h"
#include "vpi_user.h"
#include "scala_vpi.h"

#include <stdlib.h>

#define JLONG2HANDLE(l) ((vpiHandle)l)
#define HANDLE2JLONG(h) ((jlong)h)

/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    handle
 * Signature: (IJ)J
 */
JNIEXPORT jlong JNICALL Java_me_ycy_verilog_vpi_Vpi_handle
(JNIEnv *env, jobject self, jint type, jlong obj) {
    vpiHandle r = vpi_handle(type, JLONG2HANDLE(obj));

    return HANDLE2JLONG(r);
}

/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    handle_by_index
 * Signature: (JI)J
 */
JNIEXPORT jlong JNICALL Java_me_ycy_verilog_vpi_Vpi_handle_1by_1index
(JNIEnv *env, jobject self, jlong obj, jint idx) {
    vpiHandle r = vpi_handle_by_index(JLONG2HANDLE(obj), idx);

    return HANDLE2JLONG(r);
}

/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    free_object
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_me_ycy_verilog_vpi_Vpi_free_1object
(JNIEnv *env, jobject self, jlong obj) {
    vpi_free_object(JLONG2HANDLE(obj));
}

/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    iterate
 * Signature: (IJ)J
 */
JNIEXPORT jlong JNICALL Java_me_ycy_verilog_vpi_Vpi_iterate
(JNIEnv *env, jobject self, jint prop, jlong obj) {
    return HANDLE2JLONG(vpi_iterate(prop, JLONG2HANDLE(obj)));
}

/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    scan
 * Signature: (J)J
 */
JNIEXPORT jlong JNICALL Java_me_ycy_verilog_vpi_Vpi_scan
(JNIEnv *env, jobject self, jlong obj) {
    return HANDLE2JLONG(vpi_scan(JLONG2HANDLE(obj)));
}

static jlong put_value_scalar(JNIEnv *env, jlong obj, jobject value,
                              p_vpi_time time, jint flags)
{
    jclass cls = (*env)->GetObjectClass(env, value);
    jfieldID fid = (*env)->GetFieldID(env, cls, "scalar", "I");
    jint scalar = (*env)->GetIntField(env, value, fid);
    s_vpi_value v = { .format = vpiScalarVal, .value.scalar = scalar };

    return HANDLE2JLONG(vpi_put_value(JLONG2HANDLE(obj), &v, time, flags));
}

static jlong put_value_int(JNIEnv *env, jlong obj, jobject value,
                           p_vpi_time time, jint flags)
{
    jclass cls = (*env)->GetObjectClass(env, value);
    jfieldID fid = (*env)->GetFieldID(env, cls, "integer", "I");
    jint int_val = (*env)->GetIntField(env, value, fid);
    s_vpi_value v = { .format = vpiIntVal, .value.integer = int_val };

    return HANDLE2JLONG(vpi_put_value(JLONG2HANDLE(obj), &v, time, flags));
}

static jlong put_value_real(JNIEnv *env, jlong obj, jobject value,
                            p_vpi_time time, jint flags)
{
    jclass cls = (*env)->GetObjectClass(env, value);
    jfieldID fid = (*env)->GetFieldID(env, cls, "real", "D");
    jdouble real = (*env)->GetDoubleField(env, value, fid);
    s_vpi_value v = { .format = vpiRealVal, .value.real = real };
    vpiHandle r;

    printf("put real %f, time = %p, flags = %x, obj = %lx\n", 
           v.value.real, time, flags, obj);

    r = vpi_put_value(JLONG2HANDLE(obj), &v, time, flags);

    printf("ret = %p\n", r);

    return HANDLE2JLONG(r);
}

static jlong put_value_string(JNIEnv *env, jlong obj, jobject value,
                              p_vpi_time time, jint flags)
{
    jclass cls = (*env)->GetObjectClass(env, value);
    jfieldID fid = (*env)->GetFieldID(env, cls, "str", "Ljava/lang/String;");
    jobject jstr = (*env)->GetObjectField(env, value, fid);
    char *str = (*env)->GetStringUTFChars(env, jstr, NULL);
    s_vpi_value v = { .format = vpiRealVal, .value.str = str };
    vpiHandle ret = vpi_put_value(JLONG2HANDLE(obj), &v, time, flags);
    (*env)->ReleaseStringUTFChars(env, jstr, str);

    return HANDLE2JLONG(ret);
}

static jlong put_value_vector(JNIEnv *env, jlong obj, jobject value,
                              p_vpi_time time, jint flags)
{
    int i,j;
    vpiHandle ret;

    jclass cls = (*env)->GetObjectClass(env, value);
    jfieldID fid = (*env)->GetFieldID(env, cls, "vector", "[I");
    jobject vector = (*env)->GetObjectField(env, value, fid);
    jsize len = (*env)->GetArrayLength(env, vector);

    int nwords = len / 32 + ((len % 32) ? 1 : 0);
    s_vpi_vecval *vecval = malloc(nwords * sizeof(s_vpi_vecval));
    s_vpi_value val = { .format = vpiVectorVal, .value.vector = vecval };

    jboolean iscopy;
    jint *jvec = (*env)->GetIntArrayElements(env, vector, &iscopy);
   
    // printf("nwords = %d, len = %d\n", nwords, len);

    for (i = 0; i < nwords; ++i) {
        vecval[i].aval = vecval[i].bval = 0;
        for (j = 0; j < 32; ++j) {
            int k = 32 * i + j;
            if (k >= len) break;
            vecval[i].aval |= (jvec[k] & 1) << j;
            vecval[i].bval |= ((jvec[k] & 2) >> 1) << j;

            // printf("vec[%d] = %d\n", k, jvec[k]);
        }
    }

    if (iscopy) (*env)->ReleaseIntArrayElements(env, vector,
                                                jvec, JNI_ABORT);    
    ret = vpi_put_value(JLONG2HANDLE(obj), &val, time, flags);

    free(vecval);

    return ret;
}

/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    put_value
 * Signature: (JL VPI_PKG /VpiValue;L VPI_PKG /VpiValue$Time;I)J
 */
JNIEXPORT jlong JNICALL Java_me_ycy_verilog_vpi_Vpi_put_1value
  (JNIEnv *env, jobject self, jlong obj, jobject value,
   jobject time, jint flags) {
    jclass cls;
    jmethodID get_format;
    int format;
    jlong ret = 0;
    s_vpi_time s_time;
    p_vpi_time p_time = &s_time;

    if (time == NULL) {
        p_time = NULL;
    }
    else {
        /* TODO, fill s_time */
    }

    cls = (*env)->GetObjectClass(env, value);
    get_format = (*env)->GetMethodID(env, cls, "getFormat", "()I");
    format = (*env)->CallIntMethod(env, value, get_format);

    switch (format) {
    case vpiScalarVal:
        ret = put_value_scalar(env, obj, value, p_time, flags);
        break;
    case vpiIntVal:
        ret = put_value_int(env, obj, value, p_time, flags);
        break;
    case vpiRealVal:
        ret = put_value_real(env, obj, value, p_time, flags);
        break;
    case vpiStringVal:
        ret = put_value_string(env, obj, value, p_time, flags);
        break;
    case vpiVectorVal:
        ret = put_value_vector(env, obj, value, p_time, flags);
        break;
    default:
        printf("Unsupport value format(%d)\n", format);
        return 0;
    }

    return ret;
}


static jobject get_value_scalar(JNIEnv *env, jlong obj)
{
    jclass klass = (*env)->FindClass(env, VPI_PKG "/VpiValue$Scalar");
    jmethodID constr = (*env)->GetMethodID(env, klass, "<init>", "(I)V");
    s_vpi_value val = { .format = vpiScalarVal };
    jobject ret;

    vpi_get_value(JLONG2HANDLE(obj), &val);
    ret = (*env)->NewObject(env, klass, constr, val.value.scalar);

    return ret;
}

static jobject get_value_int(JNIEnv *env, jlong obj)
{
    jclass klass = (*env)->FindClass(env, VPI_PKG "/VpiValue$Int");
    jmethodID constr = (*env)->GetMethodID(env, klass, "<init>", "(I)V");
    s_vpi_value val = { .format = vpiIntVal };
    jobject ret;

    vpi_get_value(JLONG2HANDLE(obj), &val);
    ret = (*env)->NewObject(env, klass, constr, val.value.integer);

    return ret;
}

static jobject get_value_real(JNIEnv *env, jlong obj)
{
    jclass klass = (*env)->FindClass(env, VPI_PKG "/VpiValue$Real");
    jmethodID constr = (*env)->GetMethodID(env, klass, "<init>", "(D)V");
    s_vpi_value val = { .format = vpiRealVal };
    jobject ret;

    vpi_get_value(JLONG2HANDLE(obj), &val);
    ret = (*env)->NewObject(env, klass, constr, val.value.real);
    printf("get real %lx\n", obj);
    return ret;
}

static jobject get_value_string(JNIEnv *env, jlong obj)
{
    jclass klass = (*env)->FindClass(env, VPI_PKG "/VpiValue$String");
    jmethodID constr = (*env)->GetMethodID(env, klass, "<init>",
                                           "(Ljava/lang/String;)V");
    s_vpi_value val = { .format = vpiStringVal };
    jobject ret;

    vpi_get_value(JLONG2HANDLE(obj), &val);
    ret = (*env)->NewObject(env, klass, constr,
                            (*env)->NewStringUTF(env, val.value.str));

    return ret;
}

static jobject get_value_vector(JNIEnv *env, jlong obj)
{
    jclass klass = (*env)->FindClass(env, VPI_PKG "/VpiValue$Vector");
    jmethodID constr = (*env)->GetMethodID(env, klass, "<init>", "(I)V");
    jmethodID set = (*env)->GetMethodID(env, klass, "set", "(II)V");
    s_vpi_value val = { .format = vpiVectorVal };
    vpiHandle handle = JLONG2HANDLE(obj);
    jobject ret;
    int width;
    int nwords;
    int i, j;
    
    // get width
    width = vpi_get(vpiSize, handle);
    nwords = width / 32 + ((width % 32) ? 1 : 0);

    // printf("width = %d\n", width);
    ret = (*env)->NewObject(env, klass, constr, width);

    // set value
    vpi_get_value(handle, &val);
    for (i = 0; i < nwords; ++i) {
        s_vpi_vecval v = val.value.vector[i];
        for (j = 0; j < 32; ++j) {
            if ((i * 32 + j) >= width) break;
            int s = ((v.aval & (1 << j)) ? 1 : 0) +
                ((v.bval & (1 << j)) ? 2 : 0);
            // printf("val[%d] = %d\n", i * 32 + j, s);
            (*env)->CallVoidMethod(env, ret, set, i * 32 + j, s);
        }
    }

    return ret;
}

/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    get_value
 * Signature: (JI)Lme/ycy/verilog/vpi/VpiValue;
 */
JNIEXPORT jobject JNICALL Java_me_ycy_verilog_vpi_Vpi_get_1value
(JNIEnv *env, jobject self, jlong obj, jint format) {
    jobject null = (*env)->NewGlobalRef(env, NULL);

    switch (format) {
    case vpiBinStrVal:
    case vpiOctStrVal:
    case vpiDecStrVal:
    case vpiHexStrVal:
        /* TODO */
        fprintf(stderr, "unsupport format(%d) for get_value\n", format);
        return null;
    case vpiScalarVal:
        return get_value_scalar(env, obj);
    case vpiIntVal:
        return get_value_int(env, obj);
    case vpiRealVal:
        return get_value_real(env, obj);
    case vpiStringVal:
        return get_value_string(env, obj);
    case vpiVectorVal:
        return get_value_vector(env, obj);
    case vpiStrengthVal:
    case vpiTimeVal:
    case vpiObjTypeVal:
    case vpiSuppressVal:
    default:
        fprintf(stderr, "unsupport format(%d) for get_value.\n", format);
    }

    return NULL;
}


/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    get
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_me_ycy_verilog_vpi_Vpi_get
(JNIEnv *env, jobject self, jint prop, jlong obj) {
    return vpi_get(prop, JLONG2HANDLE(obj));
}

/*
 * Class:     me_ycy_verilog_vpi_Vpi
 * Method:    get_str
 * Signature: (IJ)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_me_ycy_verilog_vpi_Vpi_get_1str
(JNIEnv *env, jobject self, jint prop, jlong obj) {
    char *str = vpi_get_str(prop, JLONG2HANDLE(obj));
    return (*env)->NewStringUTF(env, str);
}
