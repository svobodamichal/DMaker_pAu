#include <iostream>
#include "TFile.h"
#include "TH1F.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TString.h"
#include "TROOT.h"
#include "TChain.h"
#include "TNtuple.h"

using namespace std;


void EventStatistics()
{

    TFile *f1 = TFile::Open("merged_output_3_000.root");
    TList *list1 = (TList*)f1->Get("picoD0AnaMaker;1");
    TH1F *hEventStat1 = (TH1F*)list1->FindObject("hEventStat1");

    TFile *f2 = TFile::Open("merged_output_3_001.root");
    TList *list2 = (TList*)f2->Get("picoD0AnaMaker;1");
    TH1F *hEventStat2 = (TH1F*)list2->FindObject("hEventStat1");

    TFile *f3 = TFile::Open("merged_output_3_002.root");
    TList *list3 = (TList*)f3->Get("picoD0AnaMaker;1");
    TH1F *hEventStat3 = (TH1F*)list3->FindObject("hEventStat1");

    TFile *f4 = TFile::Open("merged_output_3_003.root");
    TList *list4 = (TList*)f4->Get("picoD0AnaMaker;1");
    TH1F *hEventStat4 = (TH1F*)list4->FindObject("hEventStat1");

    TFile *f5 = TFile::Open("merged_output_3_004.root");
    TList *list5 = (TList*)f5->Get("picoD0AnaMaker;1");
    TH1F *hEventStat5 = (TH1F*)list5->FindObject("hEventStat1");




    TList *list = new TList;
    list->Add(hEventStat1);
    list->Add(hEventStat2);
    list->Add(hEventStat3);
    list->Add(hEventStat4);
    list->Add(hEventStat5);





    TCanvas *c1 = new TCanvas("c1", "Event Statistics", 1500, 1000);

//    TPaveLabel *t = new TPaveLabel(0.0, 0.9, 0.3, 1.0, "", "brNDC");
//    t->SetBorderSize(0);

    c1->SetGrid(0,0);
    gStyle->SetOptTitle(kFALSE);
    gStyle->SetOptDate(0);
    c1->Update();




    TH1F *hEventStatFull = (TH1F*)hEventStat1->Clone("hEventStat1");
    hEventStatFull->Reset();
    hEventStatFull->Merge(list);
    hEventStatFull->Draw();
    hEventStatFull->SetTitle(" ; ");
    hEventStatFull->SetStats(0);



    c1->SaveAs(Form("EventStatistics.pdf"));
    c1->Close();


    cout<<"Michale jsi dobrej"<<endl;
    cout<<"Hotovo, Jarvis"<<endl;

}

