add_library(pdfmaker
    detail/hpdf_guard.cpp
    error.cpp
    color.cpp
    font.cpp
    style.cpp
    page.cpp
    document.cpp
    layout/text_block.cpp
    layout/heading_block.cpp
    layout/table_block.cpp
    layout/image_block.cpp
    layout/list_block.cpp
    layout/code_block.cpp
    layout/page_break.cpp
    layout/toc_block.cpp
    layout/header_footer.cpp
)

target_include_directories(pdfmaker
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# add libharu include dirs
if(TARGET hpdf)
    target_link_libraries(pdfmaker PUBLIC hpdf)
    target_include_directories(pdfmaker PUBLIC
        $<BUILD_INTERFACE:${libharu_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${libharu_BINARY_DIR}/include>
    )
elseif(TARGET haru)
    target_link_libraries(pdfmaker PUBLIC haru)
endif()

target_compile_features(pdfmaker PUBLIC cxx_std_17)
