/*++
Copyright (c) Microsoft Corporation

Module Name:
- outputStream.hpp

Abstract:
- Classes to process text written into the console on the attached application's output stream (usually STDOUT).

Author:
- Michael Niksa <miniksa> July 27 2015
--*/

#pragma once

#include "..\terminal\adapter\adaptDefaults.hpp"
#include "..\types\inc\IInputEvent.hpp"
#include "..\inc\conattrs.hpp"
#include "IIoProvider.hpp"

class SCREEN_INFORMATION;

// The WriteBuffer class provides helpers for writing text into the TextBuffer that is backing a particular console screen buffer.
class WriteBuffer : public Microsoft::Console::VirtualTerminal::AdaptDefaults
{
public:
    WriteBuffer(_In_ Microsoft::Console::IIoProvider& io);

    // Implement Adapter callbacks for default cases (non-escape sequences)
    void Print(const wchar_t wch);
    void PrintString(_In_reads_(cch) wchar_t* const rgwch, const size_t cch);
    void Execute(const wchar_t wch);

    [[nodiscard]]
    NTSTATUS GetResult() { return _ntstatus; };

private:
    void _DefaultCase(const wchar_t wch);
    void _DefaultStringCase(_In_reads_(cch) wchar_t* const rgwch, const size_t cch);

    Microsoft::Console::IIoProvider& _io;
    NTSTATUS _ntstatus;
};

#include "..\terminal\adapter\conGetSet.hpp"


// The ConhostInternalGetSet is for the Conhost process to call the entrypoints for its own Get/Set APIs.
// Normally, these APIs are accessible from the outside of the conhost process (like by the process being "hosted") through
// the kernelbase/32 exposed public APIs and routed by the console driver (condrv) to this console host.
// But since we're trying to call them from *inside* the console host itself, we need to get in the way and route them straight to the
// v-table inside this process instance.
class ConhostInternalGetSet final : public Microsoft::Console::VirtualTerminal::ConGetSet
{
public:
    ConhostInternalGetSet(_In_ Microsoft::Console::IIoProvider& io);

    BOOL GetConsoleScreenBufferInfoEx(_Out_ CONSOLE_SCREEN_BUFFER_INFOEX* const pConsoleScreenBufferInfoEx) const override;
    BOOL SetConsoleScreenBufferInfoEx(const CONSOLE_SCREEN_BUFFER_INFOEX* const pConsoleScreenBufferInfoEx) override;

    BOOL SetConsoleCursorPosition(const COORD coordCursorPosition) override;

    BOOL GetConsoleCursorInfo(_In_ CONSOLE_CURSOR_INFO* const pConsoleCursorInfo) const override;
    BOOL SetConsoleCursorInfo(const CONSOLE_CURSOR_INFO* const pConsoleCursorInfo) override;

    BOOL FillConsoleOutputCharacterW(const WCHAR wch, const DWORD nLength,
                                     const COORD dwWriteCoord,
                                     _Out_ DWORD* const pNumberOfCharsWritten) override;
    BOOL FillConsoleOutputAttribute(const WORD wAttribute, const DWORD nLength,
                                    const COORD dwWriteCoord,
                                    _Out_ DWORD* const pNumberOfAttrsWritten) override;

    BOOL SetConsoleTextAttribute(const WORD wAttr) override;

    BOOL PrivateSetLegacyAttributes(const WORD wAttr,
                                    const bool fForeground,
                                    const bool fBackground,
                                    const bool fMeta) override;

    BOOL SetConsoleXtermTextAttribute(const int iXtermTableEntry,
                                      const bool fIsForeground) override;

    BOOL SetConsoleRGBTextAttribute(const COLORREF rgbColor,
                                    const bool fIsForeground) override;

    BOOL WriteConsoleInputW(_Inout_ std::deque<std::unique_ptr<IInputEvent>>& events,
                            _Out_ size_t& eventsWritten) override;

    BOOL ScrollConsoleScreenBufferW(const SMALL_RECT* pScrollRectangle,
                                    _In_opt_ const SMALL_RECT* pClipRectangle,
                                    _In_ COORD coordDestinationOrigin,
                                    const CHAR_INFO* pFill) override;

    BOOL SetConsoleWindowInfo(BOOL const bAbsolute,
                              const SMALL_RECT* const lpConsoleWindow) override;

    BOOL PrivateSetCursorKeysMode(const bool fApplicationMode) override;
    BOOL PrivateSetKeypadMode(const bool fApplicationMode) override;

    BOOL PrivateAllowCursorBlinking(const bool fEnable) override;

    BOOL PrivateSetScrollingRegion(const SMALL_RECT* const srScrollMargins) override;

    BOOL PrivateReverseLineFeed() override;


    BOOL SetConsoleTitleW(_In_reads_(sCchTitleLength) const wchar_t* const pwchWindowTitle,
                          _In_ unsigned short sCchTitleLength) override;

    BOOL PrivateUseAlternateScreenBuffer() override;

    BOOL PrivateUseMainScreenBuffer() override;

    BOOL PrivateHorizontalTabSet();
    BOOL PrivateForwardTab(const SHORT sNumTabs) override;
    BOOL PrivateBackwardsTab(const SHORT sNumTabs) override;
    BOOL PrivateTabClear(const bool fClearAll) override;

    BOOL PrivateEnableVT200MouseMode(const bool fEnabled) override;
    BOOL PrivateEnableUTF8ExtendedMouseMode(const bool fEnabled) override;
    BOOL PrivateEnableSGRExtendedMouseMode(const bool fEnabled) override;
    BOOL PrivateEnableButtonEventMouseMode(const bool fEnabled) override;
    BOOL PrivateEnableAnyEventMouseMode(const bool fEnabled) override;
    BOOL PrivateEnableAlternateScroll(const bool fEnabled) override;
    BOOL PrivateEraseAll() override;

    BOOL PrivateGetConsoleScreenBufferAttributes(_Out_ WORD* const pwAttributes) override;

    BOOL PrivatePrependConsoleInput(_Inout_ std::deque<std::unique_ptr<IInputEvent>>& events,
                                    _Out_ size_t& eventsWritten) override;

    BOOL SetCursorStyle(CursorType const cursorType) override;
    BOOL SetCursorColor(COLORREF const cursorColor) override;

    BOOL PrivateRefreshWindow() override;

    BOOL PrivateSuppressResizeRepaint() override;

    BOOL PrivateWriteConsoleControlInput(_In_ KeyEvent key) override;

    BOOL GetConsoleOutputCP(_Out_ unsigned int* const puiOutputCP) override;

    BOOL IsConsolePty(_Out_ bool* const pIsPty) const override;

private:
    Microsoft::Console::IIoProvider& _io;

    BOOL _FillConsoleOutput(const USHORT usElement,
                            const ULONG ulElementType,
                            const DWORD nLength,
                            const COORD dwWriteCoord,
                            _Out_ DWORD* const pNumberWritten);
};
