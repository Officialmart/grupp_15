package com.example.alarmtimes;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

//ovanför är biblioteken som används av denna aktivitet + alla från mainactivity

public class Inputtime extends MainActivity{  //en klass med mainactity som ett arv för att få tillgång till klassvariabler

    //när aktiviteten startas visas inputtime designen
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_inputtime);
    }

    //metod för att spara ner tid och gå till inmatning av nästa tid
    public void next(View v)
    {
        EditText et = (EditText) findViewById(R.id.editText);
        times.add(et.getText().toString());
        numbsub();
        if (amount <= 0)//om sista tiden har matas in går den till skrivningsaktiviteten
        {
            Intent intentNFC = new Intent(this, NFC_tool.class);
            startActivity(intentNFC);
        }
        else
        {
            Intent intentRestart = new Intent(this, Inputtime.class);
            startActivity(intentRestart);
        }
    }
}
