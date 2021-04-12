#include "thread.h"

void __stdcall boot::thread_ext::setup()
{
	uvirt;
	
	SetConsoleTitleA(randstr(20));
	
	util::c_log::Instance().setup();
	boot::c_conf::Instance().setup();
	net::c_consock::Instance().setup();
	
	vmend;
}

void boot::c_thread::setup()
{
	uvirt;
	this->core = ([this]
	{
		boot::thread_ext::setup();
	});
	std::thread(this->core).join();
	dbglog(XorStr("[ boot executed ]\n"));
	vmend;
}

void boot::c_thread::work()
{
	while (true)
	{
		uvirt;
		const auto time = GetTickCount64();
		for (auto&& obj : this->pool)
		{
			if (obj->last_exec + obj->interval < time) continue;
			obj->last_exec = time + obj->interval;
			try 
			{
				obj->func(&obj->uid);
			}
			catch (std::exception &e)
			{
				dbglog(XorStr("[ function %04x has failed => %s ]\n"), (uint32_t)&obj->func, e.what());
			}
		}
		std::this_thread::sleep_for(10ms);
		vmend;
	}
}

bool boot::c_thread::add(thread_strc::s_thread_i* t)
{
	uvirt;
	this->pool.push_back(t);
	vmend;
	return true;	
}

bool boot::c_thread::destroy(ULONGLONG uid)
{
	uvirt;

	for (size_t i = 0; i < this->pool.size(); i++)
	{
		auto obj = this->pool[i];
		if (!obj || obj->uid != uid) continue;
		this->pool.erase(this->pool.begin() + i);
		delete obj;
		break;
	}

	vmend;	
	return true;
}
