##############################################################################
# This file is part of the TouchGFX 4.12.3 distribution.
#
# <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
# All rights reserved.</center></h2>
#
# This software component is licensed by ST under Ultimate Liberty license
# SLA0044, the "License"; You may not use this file except in compliance with
# the License. You may obtain a copy of the License at:
#                             www.st.com/SLA0044
#
##############################################################################

# Relative location of the TouchGFX framework from root of application
touchgfx_path := ../../../touchgfx

# Optional additional compiler flags
user_cflags := -DUSE_BPP=16

# Add text converter options. Valid options are:
# 'remap'/'yes' will map identical texts to the same memory area to save space
# 'A4'/'A8' is will generate fonts in the given format
# 'binary_translations' will generate binary translations instead of cpp files
# 'binary_fonts' will generate binary font files instead of cpp files
# 'BPP16' will ensure font generation relevant for 16 bpp
text_converter_options := remap A4 BPP16
