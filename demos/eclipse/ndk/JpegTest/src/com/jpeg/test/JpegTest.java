package com.jpeg.test;

import java.io.InputStream;

public class JpegTest {

	public static native void nativeTest(InputStream jinputStream, byte[] storage);
	
	static {
		System.loadLibrary("image_codec");
	}
}
