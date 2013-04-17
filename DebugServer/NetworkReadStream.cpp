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


#include "StdAfx.h"
#include "NetworkReadStream.h"


NetworkReadStream::NetworkReadStream(SOCKET socket, UINT readTimeout)
{
    m_Socket = socket;
    m_DataAvailable = 0;
    m_ReadPtr = 0;
    m_ReadTimeout.tv_sec = 0;
    m_ReadTimeout.tv_usec = readTimeout * 1000; 
}

NetworkReadStream::~NetworkReadStream(void)
{
}

bool NetworkReadStream::CheckDataAvailable(UINT timeout)
{
    if (m_DataAvailable < m_ReadPtr)
        return true;

    timeval timeval;
    timeval.tv_sec = 0;
    timeval.tv_usec = timeout * 1000; 

    fd_set readSet;
    readSet.fd_count = 1;
    readSet.fd_array[0] = m_Socket;

    int iResult = select((int)m_Socket, &readSet, NULL, NULL, &timeval);
    if (iResult > 0)
    {
        return true;
    }

    return false;
}

STDMETHODIMP NetworkReadStream::Read(__out_bcount_part(cb, *pcbRead) void *pv, __in ULONG cb, __out ULONG *pcbRead)
{
    if (!pv)
        return E_POINTER;

    if (!pcbRead)
        return E_POINTER;

    HRESULT hr = S_OK;

    BYTE *pOutput = (BYTE*)pv;
    UINT dataRead = 0;
    while (cb > 0)
    {
        if (m_DataAvailable == m_ReadPtr)
        {
            m_ReadPtr = 0;
            m_DataAvailable = 0;

            //Get next block
            fd_set readSet;
            readSet.fd_count = 1;
            readSet.fd_array[0] = m_Socket;
            int res = select((int)m_Socket, &readSet, NULL, NULL, &m_ReadTimeout);
            if (res > 0)
            {
                res = recv(m_Socket, m_Buffer, sizeof(m_Buffer), 0);
                if (res > 0)
                {
                    m_DataAvailable = res;
                }
                else if (res == 0)
                {
                    // connection closed unexpectedly
                    hr = E_UNEXPECTED;
                    break;
                }
                else
                {
                    // network error
                    hr = E_FAIL;
                    break;
                }
            }
            else
            {
                // no more data available, read would cause block.
                hr = S_FALSE;
                break;
            }
        }

        UINT dataAvailable = m_DataAvailable - m_ReadPtr;
        if (dataAvailable >= cb)
        {
            dataAvailable = cb;
        }

        memcpy(pOutput + dataRead, &(m_Buffer[m_ReadPtr]), dataAvailable);

        m_ReadPtr += dataAvailable;
        dataRead += dataAvailable;
        cb -= dataAvailable;
    }

    *pcbRead = dataRead;
    return hr;
}
