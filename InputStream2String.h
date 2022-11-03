const char* InputStream2String(JNIEnv* env, jobject inputStream){
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
    auto line = (jstring) env->CallObjectMethod(bufferedReader, readLine);
    jmethodID append = env->GetMethodID(StringBuilder, "append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;");
    while (!env->IsSameObject(line, nullptr)){
        env->CallObjectMethod(stringBuilder, append, line);
        line = (jstring) env->CallObjectMethod(bufferedReader, readLine);
    }

    jmethodID close = env->GetMethodID(BufferedReader, "close", "()V");
    env->CallVoidMethod(bufferedReader, close);

    jmethodID toString = env->GetMethodID(StringBuilder, "toString", "()Ljava/lang/String;");
    auto r = (jstring) env->CallObjectMethod(stringBuilder, toString);
    const char* string = env->GetStringUTFChars(r, nullptr);

    env->DeleteLocalRef(InputStreamReader);
    env->DeleteLocalRef(BufferedReader);
    env->DeleteLocalRef(StringBuilder);
    env->DeleteLocalRef(reader);
    env->DeleteLocalRef(line);
    env->DeleteLocalRef(bufferedReader);
    env->DeleteLocalRef(stringBuilder);
    env->DeleteLocalRef(r);
    return string;
}
