#pragma once 

#include "libevwork/EVComm.h"
#include "Request.h"

#include <memory>

namespace js
{

	class IAppContext
	{
	public:
		virtual ~IAppContext() {};

		virtual void RequestDispatch(Request& request, evwork::IConn* pConn) = 0;
	};
	class IAppContextAware
	{
	protected:
		IAppContext* m_pAppContext;
	public:
		IAppContextAware() : m_pAppContext(NULL) {}
		virtual ~IAppContextAware() {}

		void setAppContext(IAppContext* pAppContext) { m_pAppContext = pAppContext; }
		IAppContext* getAppContext() { return m_pAppContext; }
	};


	// ��MFC��Ϣӳ��ʵ��

	struct PHClass{};

	typedef void (PHClass::*TargetFunc)();

	// ����union�÷�ʵ�ֲ�ͬ���βεĺ���ת��
	union TargetProc
	{
		TargetFunc mf_oo;
		void (PHClass::*mf_vv)();
		void (PHClass::*mf_vc)(Json::Value*);
		void (PHClass::*mf_vcc)(Json::Value*, evwork::IConn*);
	};

	// �����
	class CFromHandle
	{
	public:
		virtual ~CFromHandle() {}
		virtual Json::Value* handlePacket(Request& req)
		{
			std::auto_ptr<Json::Value> obj(new Json::Value());
			
			Json::Reader reader;
			reader.parse(req.Body(), req.Body() + req.BodySize(), *obj);

			return obj.release();
		}
		virtual void destroyFrom(Json::Value* p)
		{
			if (p)
			{
				delete p;
			}
		}
	};

	// ��Ϣӳ�亯������
	enum FormProcType
	{
		fpt_vv,			// void ::()
		fpt_vc,			// void ::(Json::Value*)
		fpt_vcc,		// void ::(Json::Value*, Conn*)
	};

	// ��Ϣӳ���¼����
	struct FormEntry
	{
		uint32_t m_uCmd;
		uint32_t m_uType;
		TargetFunc m_pFunc;
		PHClass* m_pObj;
	};

	// ��Ϣӳ���

	#define DECLARE_JS_FORM_MAP \
		static js::FormEntry *getFormEntries(); \
		static js::FormEntry formEntries[];

	#define BEGIN_JS_FORM_MAP(theClass) \
		js::FormEntry* theClass::getFormEntries() { return theClass::formEntries; } \
		js::FormEntry theClass::formEntries[] = {

	#define END_JS_FORM_MAP() \
		{0, js::fpt_vv, NULL, NULL} \
		};

	#define ON_JS_REQUEST(cmd, fp) \
		{ cmd, js::fpt_vc, (js::TargetFunc)(static_cast<void (js::PHClass::*)(Json::Value*)>(fp)), NULL},

	#define ON_JS_REQUEST_CONN(cmd, fp) \
		{ cmd, js::fpt_vcc, (js::TargetFunc)(static_cast<void (js::PHClass::*)(Json::Value*, evwork::IConn*)>(fp)), NULL},
}	 
