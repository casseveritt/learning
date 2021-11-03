/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package us.xyzw.minesweeper;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

class MinesweeperView extends GLSurfaceView {
    private static final String TAG = "Minesweeper";
    private static final boolean DEBUG = true;

    public MinesweeperView(Context context) {
        super(context);
        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        setRenderer(new Renderer());
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        public void onDrawFrame(GL10 gl) {
            MinesweeperLib.step();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            MinesweeperLib.resize(width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            MinesweeperLib.init();
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent e) {
            // MotionEvent reports input details from the touch screen
            // and other input controls. In this case, you are only
            // interested in events where the touch position changed.

        float x = e.getX();
        float y = e.getY();
        Log.i(TAG, "Touch x: " + x + ", y: " + y + " type: " + e.getAction() + " ind: 0");
        MinesweeperLib.touch(x,y, e.getAction(), 0);
        if (e.getPointerCount() == 2) {
            x = e.getX(1);
            y = e.getY(1);
            Log.i(TAG, "Touch x: " + x + ", y: " + y + " type: " + e.getAction() + " ind: 1");
            MinesweeperLib.touch(x,y, e.getAction(), 1);
        }
        return true;
    }

}
