package bbm.webrtc.rtc4j.core;

import bbm.webrtc.rtc4j.core.observer.DataChannelObserver;
import bbm.webrtc.rtc4j.model.DataBuffer;
import lombok.extern.slf4j.Slf4j;

import java.io.Closeable;
import java.nio.ByteBuffer;
import java.util.concurrent.locks.ReentrantLock;

/**
 * @author bbm
 */
@Slf4j
public class DataChannel implements Closeable {

    private Long nativeObject;
    private ReentrantLock lock = new ReentrantLock(true);
    private String label = null;
    private boolean disableSend = false;

    DataChannel(long nativeObject) {
        this.nativeObject = nativeObject;
    }

    public void registerObserver(DataChannelObserver observer) {
        lock.lock();
        try {
            nullPointCheck();
            nativeRegisterObserver(nativeObject, observer);
        } finally {
            lock.unlock();
        }
    }

    public String getLabel() {
        if (label != null) {
            return label;
        } else {
            lock.lock();
            try {
                if (label == null) {
                    nullPointCheck();
                    label = nativeGetLabel(nativeObject);
                }
                return label;
            } finally {
                lock.unlock();
            }
        }
    }

    public void send(DataBuffer dataBuffer) {
        lock.lock();
        try {
            if (disableSend) {
                throw new RuntimeException("Send function is disabled, this data channel will been closed:" + label);
            }
            nullPointCheck();
            nativeSend(nativeObject, dataBuffer.getData(), dataBuffer.getBinary());
        } finally {
            lock.unlock();
        }
    }


    public boolean isOpen() {
        lock.lock();
        try {
            nullPointCheck();
            return nativeIsOpen(nativeObject);
        } finally {
            lock.unlock();
        }
    }

    /**
     *
     */
    @Override
    public void close() {
        lock.lock();
        if (nativeObject != null) {
            free(nativeObject);
            nativeObject = null;
        }
        lock.unlock();
    }

    public void disableSend() {
        lock.lock();
        if (nativeObject != null) {
            disableSend = true;
        }
        lock.unlock();
    }

    private void nullPointCheck() {
        if (nativeObject == null) {
            throw new NullPointerException("Object has been destroyed");
        }
    }

    private native void nativeSend(long nativeObject, ByteBuffer buffer, boolean binary);

    private native void nativeRegisterObserver(long nativeObject, DataChannelObserver observer);

    private native String nativeGetLabel(long nativeObject);

    private native boolean nativeIsOpen(long nativeObject);

    private native void free(long address);


}
