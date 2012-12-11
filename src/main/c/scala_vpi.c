#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <string.h>
#include "me_ycy_verilog_vpi_Vpi.h"
#include "vpi_user.h"
#include "scala_vpi.h"

#ifdef WIN32
#define DELIM ";"
#else
#define DELIM ":"
#endif

#define CLASSPATH_MAX (10*1024)
#define JVM_OPT_MAX (256)

static JNIEnv *jvm_env = NULL;

#define vpi_error vpi_printf

void register_vpi_method(JNIEnv *env)
{
    JNINativeMethod nm_list[] = {
        {
            .name = "handle",
            .signature = "(IJ)J",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_handle,
        },
        {
            .name = "handle_by_index",
            .signature = "(JI)J",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_handle_1by_1index,
        },
        {
            .name = "free_object",
            .signature = "(J)V",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_free_1object,
        },
        {
            .name = "iterate",
            .signature = "(IJ)J",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_iterate,
        },
        {
            .name = "scan",
            .signature = "(J)J",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_scan,
        },
        {
            .name = "put_value",
            .signature = "(JL" VPI_PKG "/VpiValue;L" VPI_PKG "/VpiValue$Time;I)J",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_put_1value,
        },
        {
            .name = "get_value",
            .signature = "(JI)L" VPI_PKG "/VpiValue;",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_get_1value,
        },
        {
            .name = "get",
            .signature = "(IJ)I",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_get,
        },
        {
            .name = "get_str",
            .signature = "(IJ)Ljava/lang/String;",
            .fnPtr = Java_me_ycy_verilog_vpi_Vpi_get_1str,
        },
    };
    jclass vpiClass;
    int i;

    vpiClass = (*env)->FindClass(env, VPI_PKG "/Vpi");

    for (i = 0; i < sizeof(nm_list) / sizeof(JNINativeMethod); ++i) {
        (*env)->RegisterNatives(env, vpiClass, nm_list + i, 1);
    }
}

PLI_INT32 jvm_start_compile(PLI_BYTE8 *user_data)
{
    /* all argument must be string */
}

int classpath_opt(char *ret, int max_len)
{
    int len;
    char *scala_home;
    char *classpath;

    if (scala_home = getenv("SCALA_HOME")) {
        classpath = getenv("CLASSPATH");
        if (classpath == NULL) {
            classpath = ".";
        }

        len = snprintf(ret, max_len,
                       "-Djava.class.path="
                       "%s/lib/scala-library.jar" DELIM
                       "%s/lib/scalap.jar" DELIM
                       "%s/lib/scala-swing.jar" DELIM
                       "." DELIM
                       "%s",
                       scala_home, scala_home, scala_home,
                       classpath);
        if (len == max_len) {
            vpi_error("environment variable SCALA_HOME too long.");
            return 0;
        }
    }
    else {
        vpi_error("please set SCALA_HOME environment variable.");
        return 0;
    }

    return 1;
}

JavaVMOption *extract_jvm_options(int *n)
{
    JavaVMOption *opts;

    // vpi var */
    vpiHandle tfh;
    vpiHandle iter;
    vpiHandle argh;
    s_vpi_value v;

    int argc;
    int i;
    char *cp_opt;

    tfh = vpi_handle(vpiSysTfCall, NULL);

    opts = malloc(JVM_OPT_MAX * sizeof(JavaVMOption *));
    if (opts == NULL) goto end;

    /* append class path */
    cp_opt = malloc(CLASSPATH_MAX);
    if (cp_opt == NULL) {
        free(opts);
        opts = NULL;
        goto end;
    }
    if (!classpath_opt(cp_opt, CLASSPATH_MAX)) {
        free(cp_opt);
        free(opts);
        opts = NULL;
        goto end;
    }
    opts[0].optionString = cp_opt;

    /* append other arguments */
    i = 1;
    iter = vpi_iterate(vpiArgument, tfh);
    while (argh = vpi_scan(iter)) {
        int len;
        char *str;

        v.format = vpiStringVal;
        vpi_get_value(argh, &v);

        len = strlen(v.value.str);
        str = malloc(len);
        if ((str == NULL) || (i >= JVM_OPT_MAX)) {
            int j;
            for (j = 0; j < i; ++j) {
                free(opts[j].optionString);
            }
            free(opts);
            opts = NULL;
            goto end;
        }

        strcpy(str, v.value.str);
        opts[i].optionString = str;

        i++;
    }
    *n = i;

  end:
    return opts;
}

void free_jvm_options(JavaVMOption *opts, int n)
{
    int i;

    for (i = 0; i < n; ++i) free(opts[i].optionString);
    free(opts);
}

PLI_INT32 jvm_start_call(PLI_BYTE8 *user_data)
{
    int i;

    JavaVM *jvm;
    JNIEnv *env;
    JavaVMInitArgs args;
    JavaVMOption *options;
    int option_num;

    if (jvm_env != NULL) {
        vpi_printf("ERROR, JVM already started.");
        return 0;
    }

    /* copy options */
    options = extract_jvm_options(&option_num);
    if (options == NULL) {
        return 1;
    }

    /*
    for (i = 0; i < option_num; ++i) {
        printf("opt[%d] = %s\n", i, options[i].optionString);
    }
    */

    args.options = options;
    args.nOptions = option_num;
    args.version = JNI_VERSION_1_4;
    args.ignoreUnrecognized = JNI_TRUE;

    JNI_CreateJavaVM(&jvm, (void **)&env, &args);

    free_jvm_options(options, option_num);

    jvm_env = env;

    register_vpi_method(env);

    return 0;
}

PLI_INT32 jvm_invoke_compile(PLI_BYTE8 *data)
{
    // must 2 string args
    vpiHandle tfh, argh;
    int i = 0;
    vpiHandle iter;
    s_vpi_value val;

    int retval = 0;

    tfh = vpi_handle(vpiSysTfCall, NULL);
    iter = vpi_iterate(vpiArgument, tfh);

    while (argh = vpi_scan(iter)) {
        if (i > 2) {
            vpi_error("jvm_invoke need 2 argument.\n");
            retval = 1;
            vpi_free_object(iter);
            goto err;
        }
        i++;
    }

    if (i != 2) {
        vpi_error("jvm_invoke need 2 argument.\n");
        retval = 1;
        goto err;
    }

  err:
    return retval;
}

/* arg0 = class, arg1 = static method */
PLI_INT32 jvm_invoke_call(PLI_BYTE8 *data)
{
    /* jni var */
    jclass cls;
    jmethodID mid;
    JNIEnv *env = jvm_env;

    /* vpi var */
    vpiHandle tfh, argh, iter;
    s_vpi_value v;
    int i;

    if (env == NULL) {
        vpi_error("jvm not start!\n");
        return 1;
    }

    tfh = vpi_handle(vpiSysTfCall, NULL);
    iter = vpi_iterate(vpiArgument, tfh);

    argh = vpi_scan(iter);
    v.format = vpiStringVal;
    vpi_get_value(argh, &v);


    cls = (*env)->FindClass(env, v.value.str);
    if (!cls) {
        vpi_error("Can't find class %s\n", v.value.str);
        return 1;
    }

    argh = vpi_scan(iter);
    v.format = vpiStringVal;
    vpi_get_value(argh, &v);

    mid = (*env)->GetStaticMethodID(env, cls, v.value.str, "()V");
    if (!mid) {
        vpi_error("Can't find static method %s\n", v.value.str);
        return 1;
    }

    (*env)->CallStaticVoidMethod(env, cls, mid);

    return 0;
}

PLI_INT32 jvm_task(PLI_BYTE8 *data)
{
    /* jni var */
    jclass cls;
    jmethodID mid;
    JNIEnv *env = jvm_env;
    jstring task;

    /* vpi var */
    vpiHandle tfh, argh, iter;
    s_vpi_value v;
    int i;

    if (env == NULL) {
        vpi_error("jvm not start!\n");
        return 1;
    }

    cls = (*env)->FindClass(env, VPI_PKG "/SysTask");
    if (!cls) {
        vpi_error("Can't find class %s\n", VPI_PKG "/SysTask");
        return 1;
    }

    mid = (*env)->GetStaticMethodID(env, cls,
                                    "call",
                                    "(Ljava/lang/String;)Z");
    if (!mid) {
        vpi_error("Can't find static method %s\n", "call");
        return 1;
    }

    tfh = vpi_handle(vpiSysTfCall, NULL);
    iter = vpi_iterate(vpiArgument, tfh);

    argh = vpi_scan(iter);
    v.format = vpiStringVal;
    vpi_get_value(argh, &v);

    task = (*env)->NewStringUTF(env, v.value.str);
    (*env)->CallStaticBooleanMethod(env, cls, mid, task);
    (*env)->DeleteLocalRef(env, task);

    return 0;
}

void register_all()
{
    s_vpi_systf_data t_jvm_start = {
        .type = vpiSysTask,
        .tfname = "$jvm_start",
        .calltf = jvm_start_call,
        .compiletf = jvm_start_compile,
        .sizetf = NULL,
        .user_data = NULL,
    };

    s_vpi_systf_data t_jvm_invoke = {
        .type = vpiSysTask,
        .tfname = "$jvm_invoke",
        .calltf = jvm_invoke_call,
        .compiletf = jvm_invoke_compile,
        .sizetf = NULL,
        .user_data = NULL,
    };

    s_vpi_systf_data t_jvm_task = {
        .type = vpiSysTask,
        .tfname = "$jvm_task",
        .calltf = jvm_task,
        .compiletf = NULL,
        .sizetf = NULL,
        .user_data = NULL,
    };

    vpi_register_systf(&t_jvm_start);
    vpi_register_systf(&t_jvm_invoke);
    vpi_register_systf(&t_jvm_task);
}

// Register the new system task here
void (*vlog_startup_routines[ ] ) () = {
    register_all,
    NULL
};
