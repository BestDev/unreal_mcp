#!/usr/bin/env python3
"""
Unreal MCP Server - FastMCP 기반 서버
Claude Code와 언리얼 엔진 플러그인 간의 MCP 브릿지 역할 수행

✅ 확인됨: FastMCP API는 D:\_Source\modelcontextprotocol\fastmcp\src\fastmcp\server\server.py에서 검증
✅ 확인됨: @mcp.tool 데코레이터 사용법 (라인 962-1078)
✅ 확인됨: mcp.run() STDIO 모드 지원 (라인 370-389)
"""

import asyncio
import aiohttp
from fastmcp import FastMCP

# ✅ 확인됨: FastMCP 클래스 인스턴스 생성 (server.py:129)
mcp = FastMCP(name="unreal_mcp")


@mcp.tool
async def add_blueprint_node(blueprint_path: str, node_type: str = "PrintString") -> dict:
    """
    언리얼 엔진 블루프린트에 노드를 추가합니다.

    ✅ 확인됨: FastMCP @mcp.tool 데코레이터는 JSON-RPC 파라미터를 자동으로 매핑
    - Claude Code에서 보내는 JSON: {"inputs": {"blueprint_path": "...", "node_type": "..."}}
    - FastMCP가 자동으로 함수 매개변수로 변환

    Args:
        blueprint_path (str): 블루프린트 애셋 경로 (예: "/Game/MyBP")
        node_type (str): 추가할 노드 타입 (기본값: "PrintString")

    Returns:
        dict: 노드 추가 결과
    """
    try:
        # 언리얼 플러그인 HTTP 서버에 요청 전송
        # ✅ 확인됨: aiohttp는 비동기 HTTP 클라이언트 라이브러리
        async with aiohttp.ClientSession() as session:
            payload = {
                "blueprint_path": blueprint_path,
                "node_type": node_type
            }

            # ✅ 확인됨: spec.md에 따라 언리얼 플러그인은 포트 8080에서 HTTP 서버 운영
            async with session.post(
                "http://localhost:8080/add_node",
                json=payload,
                timeout=aiohttp.ClientTimeout(total=10)
            ) as response:
                if response.status == 200:
                    result = await response.json()
                    return {
                        "success": True,
                        "message": f"Node '{node_type}' added to '{blueprint_path}' successfully.",
                        "data": result
                    }
                else:
                    error_text = await response.text()
                    return {
                        "success": False,
                        "error": f"HTTP {response.status}: {error_text}"
                    }

    except aiohttp.ClientConnectorError:
        return {
            "success": False,
            "error": "Cannot connect to Unreal plugin. Make sure Unreal Editor is running with the plugin enabled."
        }
    except asyncio.TimeoutError:
        return {
            "success": False,
            "error": "Request timeout. Unreal plugin may be busy or unresponsive."
        }
    except Exception as e:
        return {
            "success": False,
            "error": f"Unexpected error: {str(e)}"
        }


if __name__ == "__main__":
    # ✅ 확인됨: mcp.run()은 STDIO 모드로 실행 (Claude Code 기본 호환)
    # 소스: D:\_Source\modelcontextprotocol\fastmcp\src\fastmcp\server\server.py:370-389
    mcp.run()