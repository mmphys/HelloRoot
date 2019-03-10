/*************************************************************************************
 
 HelloRoot.cpp
 Hello World for CERN ROOT

 Source file: ./tests/Test_serialisation.cc
 
 Copyright (C) 2019
 
 Author: Michael Marshall <michael.marshall@ed.ac.uk>
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 
 See the full license in the file "LICENSE" in the top level distribution directory
 *************************************************************************************/
/*  END LEGAL */

#include <cstdio>
#include <iostream>
#include <list>

// CERN root libraries
//#include "TBox.h"
#include "TCanvas.h"
//#include "TDecompLU.h"
//#include "TDecompSVD.h"
#include "TFile.h"
//#include "TFrame.h"
//#include "GuiTypes.h"
//#include "TGraphErrors.h"
//#include "TTree.h"
#include "TF1.h"
#include "TH1D.h"
//#include "TH2D.h"
//#include "TLatex.h"
#include "TLegend.h"
//#include "TMath.h"
//#include "TMatrixD.h"
//#include "TMatrixDLazy.h"
//#include "TMultiGraph.h"
//#include "TPaveStats.h"
//#include "TProfile.h"
//#include "TRootCanvas.h"
#include "TTreeReader.h"
//#include "TVectorD.h"

TH1D * LoadHist( const char * pFileName, const char * Name, const char * Title )
{
  TH1D * h = new TH1D(Name, Title, 50, 100, 160);
  TFile fBackground(pFileName);
  TTreeReader r("tree", &fBackground);
  TTreeReaderValue<Double_t> rvInvariantMass(r, "invariantMass");
  TTreeReaderValue<Double_t> rvEventWeight(r, "eventWeight");
  while( r.Next() )
    h->Fill( *rvInvariantMass, *rvEventWeight * 100 );
  fBackground.Close();
  return h;
}

// Load the pseudo data histogram
TH1D * LoadData()
{
  TFile fData("PseudoData_Histogram_100fb.root");
  TH1D * hist = (TH1D *) fData.Get("signal");
  hist->SetDirectory(nullptr);
  return hist;
}

void RootLab1(const char * pszFileName, const char * pszTitle)
{
  constexpr double LeftMargin = 0.15;
  
  // Load the data
  TH1D * hData = LoadData();
  std::string sTitle(pszTitle);
  sTitle.append( ";X Axis;#Sigma weights / 100 fb^{-1}" );
  TH1D * hBG = LoadHist("Background_1fb.root", "Background", sTitle.c_str());
  TH1D * hS = LoadHist("Signal_1fb.root", "Signal", "Signal;X Axis;Y Axis");
  
  // Perform a fit
  TF1 *BGFit = new TF1("BGFit","gaus",100,160, TF1::EAddToList::kNo);
  BGFit->SetLineColor(kBlue);
  BGFit->SetLineWidth(2);
  BGFit->SetLineStyle(7);
  /*BGFit->SetFillColor(19);
   BGFit->SetFillStyle(0);*/
  BGFit->SetChisquare(284.1693);
  BGFit->SetNDF(47);
  /*BGFit->GetXaxis()->SetLabelFont(42);
   BGFit->GetXaxis()->SetLabelSize(0.035);
   BGFit->GetXaxis()->SetTitleSize(0.035);
   BGFit->GetXaxis()->SetTitleOffset(1);
   BGFit->GetXaxis()->SetTitleFont(42);
   BGFit->GetYaxis()->SetLabelFont(42);
   BGFit->GetYaxis()->SetLabelSize(0.035);
   BGFit->GetYaxis()->SetTitleSize(0.035);
   BGFit->GetYaxis()->SetTitleFont(42);*/
  BGFit->SetParameter(0,940899.2);
  BGFit->SetParError(0,13294.19);
  BGFit->SetParLimits(0,0,0);
  BGFit->SetParameter(1,-308.5508);
  BGFit->SetParError(1,1.334166);
  BGFit->SetParLimits(1,0,0);
  BGFit->SetParameter(2,147.5879);
  BGFit->SetParError(2,0.242574);
  BGFit->SetParLimits(2,0,167.1734);
  BGFit->SetParent(hBG);
  hBG->GetListOfFunctions()->Add(BGFit);
  
  // Now calculate difference between data and fit
  TH1D * hDiff = (TH1D *) hData->Clone("Difference");
  hDiff->Add(hBG->GetFunction("BGFit"),-1);
  
  /*{
    // Write histograms to disk so I can play with them
    TFile fTmp("tmp.root","RECREATE");
    hBG->Write();
    hS->Write();
    hDiff->Write();
  }*/
  
  // Add the background to the signal
  TH1D * hSigPlusBg = (TH1D *) hS->Clone("SigPlusBg");
  hSigPlusBg->Add(hBG,1);
  
  // Fit for the difference
  //TF1 *DiffFit = new TF1("PrevFitTMP","gaus",116.8,146.8, TF1::EAddToList::kNo);
  TF1 *DiffFit = new TF1("PrevFitTMP","gaus",100,160, TF1::EAddToList::kNo);
  DiffFit->SetFillColor(19);
  DiffFit->SetFillStyle(0);
  DiffFit->SetLineColor(kCyan+3);
  DiffFit->SetLineWidth(2);
  DiffFit->SetChisquare(207952.5);
  DiffFit->SetNDF(22);
  DiffFit->GetXaxis()->SetLabelFont(42);
  DiffFit->GetXaxis()->SetLabelSize(0.035);
  DiffFit->GetXaxis()->SetTitleSize(0.035);
  DiffFit->GetXaxis()->SetTitleOffset(1);
  DiffFit->GetXaxis()->SetTitleFont(42);
  DiffFit->GetYaxis()->SetLabelFont(42);
  DiffFit->GetYaxis()->SetLabelSize(0.035);
  DiffFit->GetYaxis()->SetTitleSize(0.035);
  DiffFit->GetYaxis()->SetTitleFont(42);
  DiffFit->SetParameter(0,462.5305);
  DiffFit->SetParError(0,0.9648578);
  DiffFit->SetParLimits(0,0,0);
  DiffFit->SetParameter(1,124.3348);
  DiffFit->SetParError(1,0.002993755);
  DiffFit->SetParLimits(1,0,0);
  DiffFit->SetParameter(2,1.224602);
  DiffFit->SetParError(2,0.00324882);
  DiffFit->SetParLimits(2,0,35.39291);
  DiffFit->SetParent(hDiff);
  hDiff->GetListOfFunctions()->Add(DiffFit);
  
  // Define the Canvas
  TCanvas *c = new TCanvas("c", "canvas", 800, 800);
  
  // Upper plot will be in padTop
  TPad *padTop = new TPad("padTop", "padTop", 0, 0.3, 1, 1);
  padTop->SetLeftMargin(LeftMargin);
  padTop->SetBottomMargin(0);
  padTop->SetGridx();
  padTop->Draw();
  padTop->cd();
  
  // Draw
  hBG->SetStats(0);
  hBG->SetLineColor(kWhite);
  //hBG->SetLineStyle(2);
  hBG->SetLineWidth(0);
  hBG->Draw("func");//"c hist");
  
  //hS->SetLineColor(kMagenta);
  //hS->SetLineWidth(2);
  //hS->Draw("c hist same");
  
  hSigPlusBg->SetLineColor(kRed);
  hSigPlusBg->SetLineWidth(2);
  hSigPlusBg->Draw("c hist same");
  
  hData->SetMarkerStyle(kFullCircle);
  hData->SetMarkerColor(kBlack);
  hData->SetMarkerSize(0.6);
  hData->Draw("X0 E0 SAME");
  
  // Now add a legend
  //gPad->BuildLegend();
  auto legend = new TLegend(0.60,0.7,0.9,0.9);
  legend->SetHeader("H #rightarrow #gamma #gamma","C");
  legend->AddEntry(hData,"Data","lep");
  legend->AddEntry(hSigPlusBg,"Signal+background","l");
  legend->AddEntry(BGFit,"Background","l");
  legend->Draw();
  
  // Do not draw the Y axis label on the upper plot and redraw a small
  // axis instead, in order to avoid the first label (0) to be clipped.
  /*hBG->GetYaxis()->SetLabelSize(0.);
   TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
   axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
   axis->SetLabelSize(15);
   axis->Draw();*/
  
  // Now draw the lower plot
  c->cd();          // Go back to the main canvas before defining padBottom
  TPad *padBottom = new TPad("padBottom", "padBottom", 0, 0.05, 1, 0.3);
  padBottom->SetLeftMargin(LeftMargin);
  padBottom->SetTopMargin(0);
  padBottom->SetBottomMargin(0.2);
  padBottom->SetGridx();
  padBottom->Draw();
  padBottom->cd();
  
  hDiff->SetStats(0);
  hDiff->SetMarkerStyle(kFullCircle);
  hDiff->SetMarkerColor(kBlack);
  hDiff->SetMarkerSize(0.6);
  hDiff->GetYaxis()->SetLabelSize(0.05);
  hDiff->GetXaxis()->SetLabelSize(0.08);
  hDiff->SetXTitle("M_{#gamma#gamma} / GeV");
  hDiff->GetXaxis()->SetTitleSize(0.08);
  hDiff->Draw("X0 E0 func");//"c hist");
  
  // Save as .pdf
  c->Print(pszFileName);
}

// Return true if the next argument character exists
// Can skip over spaces between parameters, but won't move through another switch
bool IsNextArgChar(int argc, const char * * argv, int &i, int &j) {
  while( i < argc && argv[i][j] == 0 ) {
    if( argv[i+1][0] == '-' )
      return false;
    i++;
    j = 0;
  }
  return (i < argc);
}

int main(int argc, const char * argv[]) {
  const char * pszTestFile = nullptr;
  const char * pszTitle = nullptr;
  int iReturnValue = 0;
  std::list<const char *> Arguments;
  // Decode command line
  for( int i = 1 ; iReturnValue == 0 && i < argc ; i++ ) {
    int j = 0;
    if( argv[i][j++] != '-' )
      Arguments.push_back(argv[i]);
    else {
      switch( toupper( argv[i][j++] ) ) {
        case 'F':
          iReturnValue = 2;
          if(!IsNextArgChar(argc, argv, i, j))
            std::cout << "Error: " << argv[i] << " specified without test_filename" << std::endl;
          else if(pszTestFile)
            std::cout << "Error: test_filename specified multiple times" << std::endl;
          else {
            pszTestFile = &argv[i][j];
            std::cout << "test_filename=\"" << pszTestFile << "\"" << std::endl;
            iReturnValue = 0;
          }
          break;
        case 'T':
          iReturnValue = 3;
          if(!IsNextArgChar(argc, argv, i, j))
            std::cout << "Error: " << argv[i] << " specified without chart_title" << std::endl;
          else if(pszTitle)
            std::cout << "Error: chart_title specified multiple times" << std::endl;
          else {
            pszTitle = &argv[i][j];
            std::cout << "chart_title=\"" << pszTitle << "\"" << std::endl;
            iReturnValue = 0;
          }
          break;
      }
    }
  }
  // Check whether we have the right number of command line arguments
  if( Arguments.size() ){
    std::cout << "Unexpected arguments on command line:" << std::endl;
    int i = 1;
    for( auto p : Arguments )
      std::cout << "  " << i++ << " \"" << p << "\"" << std::endl;
    iReturnValue = 1;
  }
  if( iReturnValue ) {
    // Error - display usage info
    std::cout << "Usage: HelloRoot"
    << "\nOptional switches:"
    << "\n  -f filename Name of test file"
    << "\n  -t title    Title of ROOT chart"
    << std::endl;
  } else {
    // Command line OK - now run
    const char * pFileName = ( pszTestFile ) ? pszTestFile : "HelloRoot.pdf";
    const char * pTitle = ( pszTitle ) ? pszTitle : "Set title using: -t 'Title of chart'";
    std::cout << "Creating test image in \"" << pFileName << "\"" << std::endl;
    RootLab1( pFileName, pTitle );
    std::cout << "Done" << std::endl;
  }
  return iReturnValue;
}
