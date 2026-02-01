#ifndef Macro_h__
#define Macro_h__

namespace Client
{
#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define CHECK_FAILED(hr) if(FAILED(hr)) return E_FAIL
#define CHECK_NULPTR(pt) if(nullptr == pt) return E_FAIL

#define ALIGN_TO_MULTIPLIER(val, align) ((val + (align - 1)) & ~(align - 1))

#define NEXTBYTE D3D11_APPEND_ALIGNED_ELEMENT
}
#endif // !Macro_h__
