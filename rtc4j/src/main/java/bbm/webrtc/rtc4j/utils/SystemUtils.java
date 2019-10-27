package bbm.webrtc.rtc4j.utils;

import lombok.extern.slf4j.Slf4j;

import java.io.*;
import java.net.URL;

/**
 * @author bbm
 */
@Slf4j
public final class SystemUtils {

    private static final String LIB_NAME = "rtc";
    private static final String JNI_PATH_PREFIX = "jni/";
    private static final String TEMP_DIRECTORY = System.getProperty("java.io.tmpdir");
    private static boolean loaded;

    public synchronized static void loadNativeLibrary() {
        if (!loaded) {
            try {
                File extractedFile = extractLib();
                System.load(extractedFile.getPath());
                loaded = true;
            } catch (Throwable e) {
                e.printStackTrace();
                throw new java.lang.InternalError("Load library error!");
            }
        }
    }

    private static File extractLib() throws IOException {
        if (getLibFile() == null) {
            throw new java.lang.Error("Could not found lib: " + System.mapLibraryName(LIB_NAME));
        }
        try (final InputStream in = getLibFile().openStream()) {
            File outfile = new File(TEMP_DIRECTORY, System.mapLibraryName(LIB_NAME));
            outfile.createNewFile();
            try (FileOutputStream fos = new FileOutputStream(outfile)) {
                copy(in, fos);
            }
            outfile.deleteOnExit();
            return outfile;
        }

    }

    private static URL getLibFile() {
        return SystemUtils.class.getClassLoader()
                .getResource(JNI_PATH_PREFIX + System.mapLibraryName(LIB_NAME));
    }

    private static void copy(final InputStream in, final OutputStream out) throws IOException {
        byte[] tmp = new byte[1024 * 1024 * 4];
        int len;
        while (true) {
            len = in.read(tmp);
            if (len <= 0) {
                break;
            }
            out.write(tmp, 0, len);
        }
        out.flush();
    }
}
