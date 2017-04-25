using namespace RooFit;
using namespace RooStats;

void plotSummary(TString fileName="Comb_v00.root", TString dname="combData")
{
  gROOT->LoadMacro("AtlasStyle.C");
  SetAtlasStyle();

  TFile* f = new TFile(fileName);
  RooWorkspace* w = (RooWorkspace*)(f->Get("combWS"));
  RooStats::ModelConfig* mc = (RooStats::ModelConfig*)(w->obj("ModelConfig"));
//RooDataSet* m_data = w->data("obsData");
  RooDataSet* m_data = w->data(dname);
  RooSimultaneous* m_pdf = (RooSimultaneous*)mc->GetPdf();

  RooAbsCategoryLValue*  m_cat = (RooAbsCategoryLValue*)&m_pdf->indexCat();
  int numChannels = m_cat->numBins(0);

  TList* m_dataList = m_data->split( *m_cat, true );

  std::cout << "\t~~~~~~~~Begin Summary~~~~~~~~" << std::endl;

  std::cout << "\tThere are " << numChannels << " sub channels:" << std::endl;

  w->loadSnapshot("ucmles");
 
  for ( int i= 0; i < numChannels; i++ ) {
    m_cat->setBin(i);
    TString channelname=m_cat->getLabel();
    TString tag=channelname;
    tag.ReplaceAll("combCat_","");
    
    cout<<"\t --- "<<channelname<<"\t"<<tag<<endl;

    // RooAbsPdf* pdfi = m_pdf->getPdf(m_cat->getLabel());
    RooAbsPdf* pdfi = w->pdf("modelSB_"+tag);
    RooDataSet* datai = ( RooDataSet* )( m_dataList->At( i ) );
    std::cout << "\t\tIndex: " << i << ", Pdf: " << pdfi->GetName() << ", Data: " << datai->GetName()  << ", SumEntries: " << datai->sumEntries() << " NumEntries: "<< datai->numEntries() <<std::endl;
    TCanvas *c=new TCanvas(Form("c%d",i),tag,800,600);
    TH1F *hframe=new TH1F("hframe","",116, 180, 2500);
    hframe->SetMinimum(0.1);
    hframe->SetMaximum(1e4);
    hframe->Draw();
    hframe->GetXaxis()->SetTitle("m_{X} [GeV]");
    hframe->GetYaxis()->SetTitle("Events / 20 GeV");
    hframe->SetLineWidth(0);
    RooRealVar *x=(RooRealVar*)pdfi->getObservables(datai)->first();
    x->setRange(180, 2500);
    
    RooPlot *frame=x->frame();
    datai->plotOn(frame, Binning(116), DataError(RooAbsData::Poisson));

    pdfi->plotOn(frame,LineColor(kBlack));
    pdfi->plotOn(frame,Components("pdf_signal_"+tag),LineColor(kRed));
    pdfi->plotOn(frame,Components("pdf_background_"+tag),LineColor(kBlue));

    c->cd();
    frame->Draw("same");
    c->SetLogy();
    TString outputCanvasName="outputs/summary_"+tag;
    c->SaveAs(outputCanvasName+".pdf");
    c->SaveAs(outputCanvasName+".root");
  }
  std::cout << "\t~~~~~~~~~End Summary~~~~~~~~~" << std::endl;
}

