/*
 * Copyright(c) 2023-present, Atlas.
 * Distributed under the MIT License (http://opensource.org/licenses/MIT)
 */

package com.spacelion.atlas_builder

import com.spacelion.atlas_builder.build_target.BuildTargetAssembly
import com.spacelion.atlas_builder.generator.DefinitionsGenerator
import com.spacelion.tool_core.utility.DirectoryUtility
import io.github.oshai.kotlinlogging.KotlinLogging
import kotlinx.coroutines.runBlocking
import java.io.File
import java.text.SimpleDateFormat
import java.util.*

val logger = KotlinLogging.logger {}

fun main() {
    val startTimestamp = System.currentTimeMillis()
    val startTime = SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault()).format(Date())
    println("""
        -----------------$startTime-----------------
        Start running atlas build tool
    """.trimIndent())

    try {
        val assembly = BuildTargetAssembly(File(DirectoryUtility.ENGINE_ROOT_DIRECTORY))
        runBlocking {
            DefinitionsGenerator(assembly).generate()
        }
    } catch (e: Exception) {
        logger.error { "An error occurred while running the atlas builder. Please re-run it or report the error below: $e" }
    }

    val timeSpan = System.currentTimeMillis() - startTimestamp
    println("Atlas build tool finished running in ${timeSpan * 0.001} seconds")
}