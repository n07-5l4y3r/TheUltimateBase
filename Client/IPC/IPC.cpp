#include "IPC.hpp"

ts_map<uint64_t> cmds_active;
ts_queue<sCmd> cmds_pending;

extern "C" uint32_t __cdecl getCmdCB(uint64_t ui64pCmd)
{
	printf("   " "   " " > " __FUNCTION__ "\n");
	if (cmds_pending.is_empty())
		return 0ui32;
	*(sCmd*)ui64pCmd = cmds_pending.pop();
	return 0ui32;
}
extern "C" uint32_t __cdecl setPromiseCB(uint64_t ui64pCmd)
{
	printf("   " "   " " > " __FUNCTION__ "\n");
	auto pCmd = (sCmd*)ui64pCmd;
	if (!cmds_active.get(pCmd->ui64pParam).has_value())
		return 0ui32;
	cmds_active.get(pCmd->ui64pParam).value().get().set_value(pCmd->ui32Reply);
	return 0ui32;
}

std::future<uint32_t> queue_cmd(const sCmd Cmd)
{
	printf("   " " > " __FUNCTION__ "\n");
	cmds_pending.push(Cmd);
	cmds_active.add(Cmd.ui64pParam);
	return cmds_active.get(Cmd.ui64pParam).value().get().get_future();
}
