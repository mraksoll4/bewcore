package org.bewcorecore.qt;

import android.os.Bundle;
import android.system.ErrnoException;
import android.system.Os;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;

public class BewcoreQtActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        final File bitcoinDir = new File(getFilesDir().getAbsolutePath() + "/.bewcore");
        if (!bitcoinDir.exists()) {
            bitcoinDir.mkdir();
        }

        super.onCreate(savedInstanceState);
    }
}
