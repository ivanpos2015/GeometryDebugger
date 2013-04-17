//
// Copyright (C) 2013, Alojz Kovacik, http://kovacik.github.com
//
// This file is part of Geometry Debugger.
//
// Geometry Debugger is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Geometry Debugger is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Geometry Debugger. If not, see <http://www.gnu.org/licenses/>.
//


#pragma once

class NetworkReadStream
{
    SOCKET m_Socket;
    char m_Buffer[4096];
    UINT m_DataAvailable;
    UINT m_ReadPtr;
    timeval m_ReadTimeout;

public:
    NetworkReadStream(SOCKET socket, UINT readTimeout);
    ~NetworkReadStream(void);

    bool CheckDataAvailable(UINT timeout);
    
    // ISequentialStream 
    STDMETHOD(Read)(__out_bcount_part(cb, *pcbRead) void *pv, __in ULONG cb, __out ULONG *pcbRead);

    STDMETHOD(Write)( __in_bcount(cb) const void *pv, __in ULONG cb, __out ULONG *pcbWritten)
    {
        UNREFERENCED_PARAMETER(pv);
        UNREFERENCED_PARAMETER(cb);
        UNREFERENCED_PARAMETER(pcbWritten);
        return E_NOTIMPL;
    }

};

