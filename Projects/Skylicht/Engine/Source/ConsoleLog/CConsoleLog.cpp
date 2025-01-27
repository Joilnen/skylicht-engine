/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CConsoleLog.h"

namespace Skylicht
{
	CConsoleLog::CConsoleLog() :
		m_enable(false),
		m_maxLineBuffer(1000),
		m_id(0)
	{

	}

	CConsoleLog::~CConsoleLog()
	{

	}

	void CConsoleLog::setMaxLineBuffer(u32 num)
	{
		m_maxLineBuffer = num;
		while (m_logs.size() > num)
		{
			m_logs.erase(m_logs.begin());
		}
	}

	void CConsoleLog::clear()
	{
		m_logs.clear();
	}

	void CConsoleLog::write(const char *log, ELOG_LEVEL logLevel)
	{
		m_logs.push_back(SLogInfo(log, logLevel, m_id++));
		if (m_logs.size() > m_maxLineBuffer)
			m_logs.erase(m_logs.begin());
	}

	const std::string& CConsoleLog::getBuffer(bool update)
	{
		if (update)
		{
			m_buffer.clear();
			for (const SLogInfo& log : m_logs)
			{
				m_buffer += log.Log;
				m_buffer += "\n";
			}
		}

		return m_buffer;
	}
}