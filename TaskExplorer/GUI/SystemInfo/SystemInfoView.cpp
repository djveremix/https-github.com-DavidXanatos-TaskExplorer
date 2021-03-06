#include "stdafx.h"
#include "SystemInfoView.h"
#include "../TaskExplorer.h"

#include "SystemView.h"
//#include "DriversView.h"
#include "../TaskInfo/HandlesView.h"
#include "../TaskInfo/SocketsView.h"
#include "ServicesView.h"
#include "CPUView.h"
#include "RAMView.h"
#include "DiskView.h"
#include "NetworkView.h"
#include "GPUView.h"
#include "DnsCacheView.h"
#ifdef WIN32
#include "RpcView.h"
#endif

CSystemInfoView::CSystemInfoView(bool bAsWindow, QWidget* patent) 
	: CTabPanel(patent)
{
	m_bAsWindow = bAsWindow;

	setObjectName(m_bAsWindow ? "SystemWindow" : "SystemPanel");

	InitializeTabs();

	if (!m_bAsWindow)
	{
		int ActiveTab = theConf->GetValue(objectName() + "/Tabs_Active").toInt();
		QStringList VisibleTabs = theConf->GetStringList(objectName() + "/Tabs_Visible");
		RebuildTabs(ActiveTab, VisibleTabs);
	}

	connect(m_pTabs, SIGNAL(currentChanged(int)), this, SLOT(OnTab(int)));
}


CSystemInfoView::~CSystemInfoView()
{
	if (!m_bAsWindow)
	{
		int ActiveTab = 0;
		QStringList VisibleTabs;
		SaveTabs(ActiveTab, VisibleTabs);
		theConf->SetValue(objectName() + "/Tabs_Active", ActiveTab);
		theConf->SetValue(objectName() + "/Tabs_Visible", VisibleTabs);
	}
}

void CSystemInfoView::InitializeTabs()
{
	m_pSystemView = new CSystemView(this);
	AddTab(m_pSystemView, tr("System"));

	m_pCPUView = new CCPUView(this);
	AddTab(m_pCPUView, tr("CPU"));

	m_pRAMView = new CRAMView(this);
	AddTab(m_pRAMView, tr("Memory"));

	m_pGPUView = new CGPUView(this);
	AddTab(m_pGPUView, tr("GPU"));

	m_pDiskView = new CDiskView(this);
	AddTab(m_pDiskView, tr("Disk"));

	m_pAllFilesView = new CHandlesView(1, this);
	AddTab(m_pAllFilesView, tr("Files"));

	m_pNetworkView = new CNetworkView(this);
	AddTab(m_pNetworkView, tr("Network"));

	m_pAllSocketsView = new CSocketsView(true, this);
	AddTab(m_pAllSocketsView, tr("Sockets"));

	m_pDnsCacheView = new CDnsCacheView(true, this);
	AddTab(m_pDnsCacheView, tr("Dns Cache"));

	//m_pDriversView = new CDriversView(this);
	//AddTab(m_pDriversView, tr("Drivers"));

	m_pServicesView = new CServicesView(true, this);
	AddTab(m_pServicesView, tr("Services"));

#ifdef WIN32
	m_pRpcView = new CRpcView(this);
	AddTab(m_pRpcView, tr("RPC Endpoints"));
#endif
}

void CSystemInfoView::OnTab(int tabIndex)
{
	Refresh();
}

void CSystemInfoView::Refresh()
{
	QTimer::singleShot(0, m_pTabs->currentWidget(), SLOT(Refresh()));
}

void CSystemInfoView::UpdateGraphs()
{
	// todo: dont update hidden tabs
	m_pCPUView->UpdateGraphs();
	m_pRAMView->UpdateGraphs();
	m_pDiskView->UpdateGraphs();
	m_pNetworkView->UpdateGraphs();
	m_pGPUView->UpdateGraphs();
}
