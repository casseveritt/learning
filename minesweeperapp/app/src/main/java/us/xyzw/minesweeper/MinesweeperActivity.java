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

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import java.io.InputStream;
import java.io.FileOutputStream;
import android.view.WindowManager;
import java.io.IOException;

import java.io.File;

public class MinesweeperActivity extends Activity {

    private static final String TAG = "minesweeper";

    MinesweeperView mView;

    @Override protected void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        MinesweeperLib.setActivity(this);
        MinesweeperLib.setFilesDir(getFilesDir().getAbsolutePath());
        mView = new MinesweeperView(getApplication());
        setContentView(mView);
    }

    @Override protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        mView.onResume();

    }

    public boolean copyFromApk(String filename) { // Copy file out of apk
        // implement me!
        Log.i(TAG, "in Java copyFromApk( " + filename + " ) called");
        InputStream input = null;
        try {
            input = getAssets().open(filename);
            // Log.i( TAG, "apk file " + filename + " opened!" );

            File f = new java.io.File(getFilesDir().getAbsolutePath()
                    + "/" + filename);
            if (f.exists() == false) {
                // Log.i(TAG, "Creating local file " + f.getAbsolutePath() );
                f.getParentFile().mkdirs();
                f.createNewFile();
            }

            FileOutputStream output = new FileOutputStream(f);

            byte[] buf = new byte[4096];
            int len = 0;
            while ((len = input.read(buf, 0, 4096)) > 0) {
                output.write(buf, 0, len);
            }
            input.close();
            output.close();

        } catch (IOException e) {
            Log.e(TAG, "copyFromApk failure of  " + e.getMessage());
            return false;
        }

        return true;
    }
}
