"""
PlatformIO 自定义构建脚本
用于过滤 LVGL 库中不兼容 ESP32 的 ARM 特定文件
"""

Import("env")


# 从库构建中排除 ARM 特定文件
def skip_arm_files(node):
    """检查文件是否应跳过编译"""
    path = node.get_path()

    # 跳过 ARM Helium 汇编文件
    if "helium" in path and path.endswith(".S"):
        return None

    # 跳过 ARM NEON 优化
    if "neon" in path:
        return None

    # 跳过 ARM2D 绘制后端
    if "arm2d" in path:
        return None

    return node


# 应用源文件过滤器
env.AddBuildMiddleware(skip_arm_files, "*")
