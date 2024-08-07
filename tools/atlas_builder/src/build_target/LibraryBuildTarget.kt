/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

package com.spacelion.atlas_builder.build_target

import java.io.File

class LibraryBuildTarget(name: String,
                         rootDirectory: File,
                         options: Set<String>,
                         oneValueArgs : Map<String, String>,
                         multiValueArgs : Map<String, List<String>>) : BuildTargetBase(name, rootDirectory,
    options, oneValueArgs, multiValueArgs) {

    override fun exportSymbol(): Boolean  = true
}