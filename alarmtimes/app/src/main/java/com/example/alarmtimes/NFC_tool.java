package com.example.alarmtimes;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.nfc.FormatException;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.nfc.tech.Ndef;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

//ovanför är det bara olika bibliotek som används

public class NFC_tool extends MainActivity {    //en klass med Mainactivity som arv för att komma åt klassvariablerna

    //konstanter med felmedelanden
    public static final String ERROR_DETECTED = "Ingen NFC tagg hittades!";
    public static final String WRITE_SUCCESS = "Skrvning till NFC taggen fullgjordes!";
    public static final String WRITE_ERROR = "Ett fel hände under skriving, är NFC taggen tillräckligt nära enheten?";

    //variabeldeklaration
    NfcAdapter nfcAdapter;
    PendingIntent pendingIntent;
    IntentFilter writeTagFilters[];
    boolean writeMode;
    Tag myTag;
    Context context;
    Button btnWrite;

    @Override
    public void onCreate(Bundle savedInstanceState) {   //när denna aktivitet startas så kommmer nfc_tool designen visas
        super.onCreate(savedInstanceState);
        setContentView(R.layout.actitivty_nfc_tool);
        context = this;

        btnWrite = (Button) findViewById(R.id.button5);//intiterar knappen

        btnWrite.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v) {   //när knappen för att skriva till nfcn trycks så kommer det skrivas tilll en NFC tagg.
                try {
                    if(myTag ==null) {
                        Toast.makeText(context, ERROR_DETECTED, Toast.LENGTH_LONG).show();
                    } else {
                        write(printList(), myTag);
                        Toast.makeText(context, WRITE_SUCCESS, Toast.LENGTH_LONG ).show();
                    }
                } catch (IOException e) {
                    Toast.makeText(context, WRITE_ERROR, Toast.LENGTH_LONG ).show();
                    e.printStackTrace();
                } catch (FormatException e) {
                    Toast.makeText(context, WRITE_ERROR, Toast.LENGTH_LONG ).show();
                    e.printStackTrace();
                }
            }
        });

        nfcAdapter = NfcAdapter.getDefaultAdapter(this);
        if (nfcAdapter == null) {
            // Vi behöver nfc då stoppar vi om det inte finns stöd för det.
            Toast.makeText(this, "Enheten stödjer inte NFC.", Toast.LENGTH_LONG).show();
            finish();
        }

        //om en nfc tagg hittas så händer detta
        pendingIntent = PendingIntent.getActivity(this, 0, new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);
        IntentFilter tagDetected = new IntentFilter(NfcAdapter.ACTION_TAG_DISCOVERED);
        tagDetected.addCategory(Intent.CATEGORY_DEFAULT);
        writeTagFilters = new IntentFilter[] { tagDetected };
    }

    //Skriver till en tagg
    private void write(String text, Tag tag) throws IOException, FormatException {
        NdefRecord[] records = { createRecord(text) };
        NdefMessage message = new NdefMessage(records);
        //Hämtar en instans av Ndef till taggen
        Ndef ndef = Ndef.get(tag);
        //Aktiverar I/O
        ndef.connect();
        // Skriver medelandet
        ndef.writeNdefMessage(message);
        //Stänger av anslutningen till nfc
        ndef.close();
    }
    private NdefRecord createRecord(String text) throws UnsupportedEncodingException {
        String lang       = "en";
        byte[] textBytes  = text.getBytes();
        byte[] langBytes  = lang.getBytes("US-ASCII");
        int    langLength = langBytes.length;
        int    textLength = textBytes.length;
        byte[] payload    = new byte[1 + langLength + textLength];

        //Sätter status byte
        payload[0] = (byte) langLength;

        //Kopierar språkbytes och textbytes till taggen
        System.arraycopy(langBytes, 0, payload, 1,              langLength);
        System.arraycopy(textBytes, 0, payload, 1 + langLength, textLength);

        NdefRecord recordNFC = new NdefRecord(NdefRecord.TNF_WELL_KNOWN,  NdefRecord.RTD_TEXT,  new byte[0], payload);

        return recordNFC;
    }


    //startar om aktiviteten om en nfc tagg hittas för att visa ett medelandde till användaren att den hittades
    @Override
    protected void onNewIntent(Intent intent) {
        setIntent(intent);
        if(NfcAdapter.ACTION_TAG_DISCOVERED.equals(intent.getAction())){
            myTag = intent.getParcelableExtra(NfcAdapter.EXTRA_TAG);
            Toast.makeText(this,"NFC tagg hittad skriv till den!", Toast.LENGTH_LONG).show();
        }
    }

    //pausar skrivningläget
    @Override
    public void onPause(){
        super.onPause();
        WriteModeOff();
    }
    //fortsätter skrivningsläget
    @Override
    public void onResume(){
        super.onResume();
        WriteModeOn();
    }



    //Aktiverar skrivning
    private void WriteModeOn(){
        writeMode = true;
        nfcAdapter.enableForegroundDispatch(this, pendingIntent, writeTagFilters, null);
    }
    //Inaktiverar skrivning
    private void WriteModeOff(){
        writeMode = false;
        nfcAdapter.disableForegroundDispatch(this);
    }
}