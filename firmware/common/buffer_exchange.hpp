/*
 * Copyright (C) 2016 Jared Boone, ShareBrained Technology, Inc.
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#pragma once

#include "ch.h"

#include "message.hpp"
#include "fifo.hpp"
#include "io.hpp"

class BufferExchange {
public:
	BufferExchange(CaptureConfig* const config);
	~BufferExchange();

#if defined(LPC43XX_M0)
	bool empty() const {
		return fifo_buffers_for_application->is_empty();
	}

	StreamBuffer* get() {
		return get(fifo_buffers_for_application);
	}

	bool put(StreamBuffer* const p) {
		return fifo_buffers_for_baseband->in(p);
	}
#endif

#if defined(LPC43XX_M4)
	bool empty() const {
		return fifo_buffers_for_baseband->is_empty();
	}

	StreamBuffer* get() {
		return get(fifo_buffers_for_baseband);
	}

	bool put(StreamBuffer* const p) {
		return fifo_buffers_for_application->in(p);
	}
#endif

	static void handle_isr() {
		if( obj ) {
			obj->check_fifo_isr();
		}
	}

private:
	CaptureConfig* const config;
	FIFO<StreamBuffer*>* fifo_buffers_for_baseband;
	FIFO<StreamBuffer*>* fifo_buffers_for_application;
	Thread* thread;
	static BufferExchange* obj;

	void check_fifo_isr() {
		if( !empty() ) {
			wakeup_isr();
		}
	}

	void wakeup_isr() {
		auto thread_tmp = thread;
		if( thread_tmp ) {
			thread = nullptr;
			chSchReadyI(thread_tmp);
		}
	}

	StreamBuffer* get(FIFO<StreamBuffer*>* fifo);
};
