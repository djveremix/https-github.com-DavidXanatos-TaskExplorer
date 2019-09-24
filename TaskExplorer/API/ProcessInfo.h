#pragma once
#include <qobject.h>

#include "ModuleInfo.h"
#include "ThreadInfo.h"
#include "HandleInfo.h"
#include "WndInfo.h"
#include "AbstractTask.h"
#include "MiscStats.h"
#include "MemoryInfo.h"

#ifdef WIN32
#undef GetUserName
#endif

struct STaskStatsEx : STaskStats
{
	SDelta32_64 		PageFaultsDelta;
	SDelta32_64 		HardFaultsDelta;
	SDelta64 		PrivateBytesDelta;
};

class CProcessInfo: public CAbstractTask
{
	Q_OBJECT

public:
	CProcessInfo(QObject *parent = nullptr);
	virtual ~CProcessInfo();

	// Basic
	virtual quint64 GetProcessId() const				{ QReadLocker Locker(&m_Mutex); return m_ProcessId; }
	virtual quint64 GetParentId() const					{ QReadLocker Locker(&m_Mutex); return m_ParentProcessId; }
	virtual QString GetName() const						{ QReadLocker Locker(&m_Mutex); return m_ProcessName; }

	virtual bool ValidateParent(CProcessInfo* pParent) const = 0;

	virtual QString GetArchString() const = 0;
	virtual quint64 GetSessionID() const = 0;

	virtual quint16 GetSubsystem() const = 0;
	virtual QString GetSubsystemString() const = 0; // on windows wls, etc... on linux wine or not

	// Parameters
	virtual QString GetFileName() const					{ QReadLocker Locker(&m_Mutex); return m_FileName; }
	virtual QString GetCommandLineStr() const			{ QReadLocker Locker(&m_Mutex); return m_CommandLine; }
	virtual QString GetWorkingDirectory() const = 0;

	// Other fields
	virtual QString GetUserName() const					{ QReadLocker Locker(&m_Mutex); return m_UserName; }

	// Dynamic
	virtual quint32 GetNumberOfThreads() const			{ QReadLocker Locker(&m_Mutex); return m_NumberOfThreads; }
	virtual quint32 GetNumberOfHandles() const			{ QReadLocker Locker(&m_Mutex); return m_NumberOfHandles; }

	virtual quint32 GetPeakNumberOfThreads() const		{ QReadLocker Locker(&m_Mutex); return m_PeakNumberOfThreads; }
	virtual quint32 GetPeakNumberOfHandles() const = 0;

	virtual quint64 GetPeakPrivateBytes() const			{ QReadLocker Locker(&m_Mutex); return m_PeakPagefileUsage; }
	virtual quint64 GetWorkingSetSize() const			{ QReadLocker Locker(&m_Mutex); return m_WorkingSetSize; }
	virtual quint64 GetPeakWorkingSetSize() const		{ QReadLocker Locker(&m_Mutex); return m_PeakWorkingSetSize; }
	virtual quint64 GetPrivateWorkingSetSize() const	{ QReadLocker Locker(&m_Mutex); return m_WorkingSetPrivateSize; }
	virtual quint64 GetVirtualSize() const				{ QReadLocker Locker(&m_Mutex); return m_VirtualSize; }
	virtual quint64 GetPeakVirtualSize() const			{ QReadLocker Locker(&m_Mutex); return m_PeakVirtualSize; }
	//quint32 GetPageFaultCount() const					{ QReadLocker Locker(&m_Mutex); return m_PageFaultCount; }

	virtual quint64 GetSharedWorkingSetSize() const = 0;
	virtual quint64 GetShareableWorkingSetSize() const = 0;
	virtual quint64 GetMinimumWS() const = 0;
	virtual quint64 GetMaximumWS() const = 0;

	virtual STaskStatsEx GetCpuStats() const			{ QReadLocker Locker(&m_StatsMutex); return m_CpuStats; }

	virtual void SetGpuMemoryUsage(quint64 DedicatedUsage, quint64 SharedUsage) { QWriteLocker Locker(&m_StatsMutex); m_GpuDedicatedUsage = DedicatedUsage; m_GpuSharedUsage = SharedUsage; }
	virtual quint64 GetGpuDedicatedUsage() const		{ QReadLocker Locker(&m_StatsMutex); return m_GpuDedicatedUsage; }
	virtual quint64 GetGpuSharedUsage() const			{ QReadLocker Locker(&m_StatsMutex); return m_GpuSharedUsage; }

	virtual void UpdateGpuTimeDelta(quint64 totalRunningTime) { QWriteLocker Locker(&m_StatsMutex); return m_GpuTimeUsage.Delta.Update(totalRunningTime); }
	virtual void UpdateGpuTimeUsage(quint64 totalTime)	{ QWriteLocker Locker(&m_StatsMutex); return m_GpuTimeUsage.Calculate(totalTime); }
	virtual float GetGpuUsage() const					{ QReadLocker Locker(&m_StatsMutex); return m_GpuTimeUsage.Usage; }
	
	virtual void SetGpuAdapter(const QString& GpuAdapter) { QWriteLocker Locker(&m_StatsMutex); m_GpuAdapter = GpuAdapter; }
	virtual QString GetGpuAdapter() const				{ QReadLocker Locker(&m_StatsMutex); return m_GpuAdapter; }

	virtual QString GetStatusString() const = 0;

	virtual bool HasDebugger() const = 0;
	virtual STATUS AttachDebugger() = 0;
	virtual STATUS DetachDebugger() = 0;

	virtual bool IsSystemProcess() const = 0;
	virtual bool IsServiceProcess() const = 0;
	virtual bool IsUserProcess() const = 0;
	virtual bool IsElevated() const = 0;

	virtual void SetNetworkUsageFlag(quint64 uFlag)			{ QWriteLocker Locker(&m_StatsMutex); m_NetworkUsageFlags |= uFlag; }
	virtual int GetNetworkUsageFlags() const				{ QReadLocker Locker(&m_StatsMutex); return m_NetworkUsageFlags; }
	virtual QString GetNetworkUsageString() const;

	virtual SProcStats	GetStats() const					{ QReadLocker Locker(&m_StatsMutex); return m_Stats; }

	virtual CModulePtr GetModuleInfo() const				{ QReadLocker Locker(&m_Mutex); return m_pModuleInfo; }

	// Threads
	virtual QMap<quint64, CThreadPtr> GetThreadList() const	{ QReadLocker Locker(&m_ThreadMutex); return m_ThreadList; }

	// Handles
	virtual QMap<quint64, CHandlePtr> GetHandleList() const	{ QReadLocker Locker(&m_HandleMutex); return m_HandleList; }
	
	// Modules
	virtual QMap<quint64, CModulePtr> GetModuleList() const	{ QReadLocker Locker(&m_ModuleMutex); return m_ModuleList; }

	// Windows
	virtual QMap<quint64, CWndPtr>	  GetWindowList() const	{ QReadLocker Locker(&m_WindowMutex); return m_WindowList; }
	virtual CWndPtr					GetWindowByHwnd(quint64 hwnd) const { QReadLocker Locker(&m_WindowMutex); return m_WindowList.value(hwnd); }

	struct SEnvVar
	{
		enum EType
		{
			eSystem,
			eUser,
			eProcess
		};
		SEnvVar() { Type = eProcess; }
		QString Name;
		QString Value;
		EType	Type;
		QString GetTypeName() const { return GetType() + "_" + Name; }
		QString GetType() const 
		{ 
			switch (Type)
			{
				case eSystem:	return tr("System");
				case eUser:		return tr("User");
				case eProcess:	return tr("Process");
			}
			return "";
		}
	};

	virtual QMap<QString, SEnvVar>	GetEnvVariables() const = 0;
	virtual STATUS					DeleteEnvVariable(const QString& Name) = 0;
	virtual STATUS					EditEnvVariable(const QString& Name, const QString& Value) = 0;

	virtual QMap<quint64, CMemoryPtr> GetMemoryMap() const = 0;

	virtual QList<CWndPtr> GetWindows() const = 0;
	virtual CWndPtr	GetMainWindow() const = 0;
	

	virtual STATUS LoadModule(const QString& Path) = 0;

signals:
	void			ThreadsUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);
	void			HandlesUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);
	void			ModulesUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);
	void			WindowsUpdated(QSet<quint64> Added, QSet<quint64> Changed, QSet<quint64> Removed);

public slots:
	virtual bool	UpdateThreads() = 0;
	virtual bool	UpdateHandles() = 0;
	virtual bool	UpdateModules() = 0;
	virtual bool	UpdateWindows() = 0;

protected:
	// Basic
	quint64							m_ProcessId;
	quint64							m_ParentProcessId;
	QString							m_ProcessName;

	// Parameters
	QString							m_FileName;
	QString							m_CommandLine;
	//QString							m_WorkingDirectory;
	
	// Other fields
	QString							m_UserName;

	// Dynamic
	quint32							m_NumberOfThreads;
	quint32							m_NumberOfHandles;

	quint32							m_PeakNumberOfThreads;

	quint64							m_PeakPagefileUsage;
	quint64							m_WorkingSetSize;
	quint64							m_PeakWorkingSetSize;
	quint64							m_WorkingSetPrivateSize;
	quint64							m_VirtualSize;
	quint64							m_PeakVirtualSize;
	//quint32							m_PageFaultCount;

	quint32							m_NetworkUsageFlags;

	// I/O stats
	mutable QReadWriteLock			m_StatsMutex;
	SProcStats						m_Stats;
	STaskStatsEx					m_CpuStats;
	
	STimeUsage						m_GpuTimeUsage;
	quint64							m_GpuDedicatedUsage;
	quint64							m_GpuSharedUsage;
	QString							m_GpuAdapter;

	// module info
	CModulePtr						m_pModuleInfo;

	// Threads
	mutable QReadWriteLock			m_ThreadMutex;
	QMap<quint64, CThreadPtr>		m_ThreadList;

	// Handles
	mutable QReadWriteLock			m_HandleMutex;
	QMap<quint64, CHandlePtr>		m_HandleList;

	// Modules
	mutable QReadWriteLock			m_ModuleMutex;
	QMap<quint64, CModulePtr>		m_ModuleList;

	// Window
	mutable QReadWriteLock			m_WindowMutex;
	QMap<quint64, CWndPtr>			m_WindowList;

};

typedef QSharedPointer<CProcessInfo> CProcessPtr;
typedef QWeakPointer<CProcessInfo> CProcessRef;