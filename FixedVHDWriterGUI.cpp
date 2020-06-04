//
// Created by 桂明谦 on 2019/12/12.
//
#include "wx/wx.h"
#include "VHD.hpp"
#include "wx/sizer.h"
#include "wx/filedlg.h"
#include "wx/msgdlg.h"
#include "wx/textctrl.h"
#include <string>
#define VERSION "1.0"
using namespace std;

enum ITEM_ID{
    FILE_BUTTON,
    VHD_BUTTON,
    WRITE_BUTTON,
    SECTION_TEXT_AREA
};

class MyFrame:public wxFrame{
public:
    MyFrame():wxFrame(nullptr, wxID_ANY, "FixedVHD Writer", wxDefaultPosition, wxSize(500, 100)){
        SetBackgroundColour("black");
        filebutton = new wxButton(this, FILE_BUTTON, "选择要写入的文件");
        vhdbutton = new wxButton(this, VHD_BUTTON, "选择要写入的VHD文件");
        writebutton = new wxButton(this, WRITE_BUTTON, "写入");
        textctrl = new wxTextCtrl(this, SECTION_TEXT_AREA);
        textctrl->SetForegroundColour("white");
        textctrl->SetEditable(false);
        writebutton->Disable();
        wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
        this->SetSizer(sizer);
        sizer->Add(filebutton);
        sizer->AddSpacer(20);
        sizer->Add(vhdbutton);
        sizer->AddSpacer(20);
        sizer->Add(textctrl);
        sizer->Add(writebutton);
        Center();
    }
    wxDECLARE_EVENT_TABLE();

private:
    string filepath;
    string vhdpath;
    wxButton* filebutton;
    wxButton* vhdbutton;
    wxButton* writebutton;
    wxTextCtrl* textctrl;

    void IsWriteEnable(){
        if(filebutton->GetLabel()!="" && vhdbutton->GetLabel()!="")
            writebutton->Enable();
    }

    void FileButtonClick(wxCommandEvent& event){
        wxFileDialog filedlg(this, "请选择你要写入的文件");
        if(filedlg.ShowModal()!=wxID_CANCEL){
            filepath = filedlg.GetPath();
            filebutton->SetLabel(filedlg.GetFilename());
            IsWriteEnable();
        }
    }

    void VHDButtonClick(wxCommandEvent& event){
        wxFileDialog filedlg(this, "请选择你要写入的VHD文件", "", "", "VHD文件(*.vhd)|*.vhd");
        if(filedlg.ShowModal()!=wxID_CANCEL)
            vhdpath = filedlg.GetPath();
            vhdbutton->SetLabel(filedlg.GetFilename());
            FixedVHD vhd(vhdpath);
            textctrl->SetEditable(true);
            int count = vhd.GetContentSize()/512;
            textctrl->SetHint("0~"+to_string(count-1));
            IsWriteEnable();
    }

    void ShowMessage(string captain, string msg){
        wxMessageDialog dialog(this, msg, captain, wxICON_WARNING|wxOK);
        dialog.ShowModal();
    }

    void WriteButtonClick(wxCommandEvent& event){
        FixedVHD vhd(vhdpath);
        if(vhd.fail()){
            ShowMessage("error", vhdpath+" not found");
            return ;
        }
        ifstream srcfile(filepath);
        if(srcfile.fail()){
            ShowMessage("error", filepath+" not found");
            return ;
        }
        unsigned int lbacount = atoi(textctrl->GetLineText(0));
        srcfile.seekg(0, ios::end);
        const unsigned int size = srcfile.tellg();
        char buffer[size];
        srcfile.seekg(0, ios::beg);
        srcfile.read(buffer, size);
        vhd.Write(lbacount, buffer, size);
        ShowMessage("success", "wrote finished");
        writebutton->Disable();
    }
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(ITEM_ID::FILE_BUTTON, MyFrame::FileButtonClick)
EVT_BUTTON(ITEM_ID::VHD_BUTTON, MyFrame::VHDButtonClick)
EVT_BUTTON(ITEM_ID::WRITE_BUTTON, MyFrame::WriteButtonClick)
wxEND_EVENT_TABLE()

class MyApp:public wxApp{
public:
    virtual bool OnInit(){
        window = new MyFrame;
        window->Show();
        return true;
    }
private:
    MyFrame* window;
};

wxIMPLEMENT_APP(MyApp);