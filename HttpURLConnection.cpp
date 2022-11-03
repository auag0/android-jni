#include <android/log.h>
#include <jni.h>
#include <thread>

static JavaVM* javaVM;

void http(const char* _url, const char* &_response){
    std::thread([_url, &_response] () {
        JNIEnv* env;
        javaVM->AttachCurrentThread(&env, nullptr);

        jclass URL = env->FindClass("java/net/URL");
        jmethodID NEWURL = env->GetMethodID(URL, "<init>", "(Ljava/lang/String;)V");
        jobject url = env->NewObject(URL, NEWURL, env->NewStringUTF(_url));

        jmethodID openConnection = env->GetMethodID(URL, "openConnection", "()Ljava/net/URLConnection;");
        jobject httpURLConnection = env->CallObjectMethod(url, openConnection);

        jclass HttpURLConnection = env->FindClass("java/net/HttpURLConnection");
        jmethodID setRequestMethod = env->GetMethodID(HttpURLConnection, "setRequestMethod", "(Ljava/lang/String;)V");
        env->CallVoidMethod(httpURLConnection, setRequestMethod, env->NewStringUTF("GET"));

        jmethodID setReadTimeout = env->GetMethodID(HttpURLConnection, "setReadTimeout", "(I)V");
        env->CallVoidMethod(httpURLConnection, setReadTimeout, 10000);

        jmethodID connect = env->GetMethodID(HttpURLConnection, "connect", "()V");
        env->CallVoidMethod(httpURLConnection, connect);

        jmethodID getInputStream = env->GetMethodID(HttpURLConnection, "getInputStream","()Ljava/io/InputStream;");
        jobject inputStream = env->CallObjectMethod(httpURLConnection, getInputStream);

        jclass InputStreamReader = env->FindClass("java/io/InputStreamReader");
        jmethodID NEWInputStreamReader = env->GetMethodID(InputStreamReader, "<init>", "(Ljava/io/InputStream;Ljava/lang/String;)V");

        jobject reader = env->NewObject(InputStreamReader, NEWInputStreamReader, inputStream, env->NewStringUTF("utf-8"));

        jclass BufferedReader = env->FindClass("java/io/BufferedReader");
        jmethodID NEWBufferedReader = env->GetMethodID(BufferedReader, "<init>", "(Ljava/io/Reader;)V");

        jobject bufferedReader = env->NewObject(BufferedReader, NEWBufferedReader, reader);

        jclass StringBuilder = env->FindClass("java/lang/StringBuilder");
        jmethodID NEWStringBuilder = env->GetMethodID(StringBuilder, "<init>", "()V");

        jobject stringBuilder = env->NewObject(StringBuilder, NEWStringBuilder);

        jmethodID readLine = env->GetMethodID(BufferedReader, "readLine", "()Ljava/lang/String;");
        jmethodID append = env->GetMethodID(StringBuilder, "append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;");
        auto line = (jstring) env->CallObjectMethod(bufferedReader, readLine);
        while (!env->IsSameObject(line, NULL)){
            env->CallObjectMethod(stringBuilder, append, line);
            line = (jstring) env->CallObjectMethod(bufferedReader, readLine);
        }

        jmethodID close = env->GetMethodID(BufferedReader, "close", "()V");
        env->CallVoidMethod(bufferedReader, close);

        jmethodID toString = env->GetMethodID(StringBuilder, "toString", "()Ljava/lang/String;");
        auto r = (jstring) env->CallObjectMethod(stringBuilder, toString);

        _response = env->GetStringUTFChars(r, nullptr);

        env->DeleteLocalRef(URL);
        env->DeleteLocalRef(HttpURLConnection);
        env->DeleteLocalRef(InputStreamReader);
        env->DeleteLocalRef(BufferedReader);
        env->DeleteLocalRef(StringBuilder);
        env->DeleteLocalRef(url);
        env->DeleteLocalRef(httpURLConnection);
        env->DeleteLocalRef(reader);
        env->DeleteLocalRef(line);
        env->DeleteLocalRef(bufferedReader);
        env->DeleteLocalRef(stringBuilder);
        return nullptr;
    }).join();
}

jint JNI_OnLoad(JavaVM* vm, void*){
    javaVM = vm;
    JNIEnv* env;
    vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    __android_log_print(ANDROID_LOG_DEBUG, "anago", "JavaVM: %p, JNIEnv: %p", vm, env);
    const char* data;
    http("https://auag0.github.io/status.json", data);
    __android_log_print(ANDROID_LOG_DEBUG, "anago", "response:\n%s", data);
    return JNI_VERSION_1_6;
}
