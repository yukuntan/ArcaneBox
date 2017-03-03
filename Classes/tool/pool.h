/*
 * Copyright (C) 2016-2018 tan yukun  <tyk.163@163.com>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: tan yukun <tyk.163@163.com>
 */

#pragma once

#include <cstddef>
#include <cassert>
#include <vector>

namespace ECS {
	class BasePool {
	public:
		BasePool(std::size_t element_size, std::size_t chunk_size = 512)
			: chunk_size_(chunk_size), capacity_(0) {
			assert(chunk_size > element_size);
			chunk_size_ = chunk_size / element_size;
		}
		virtual ~BasePool() {}

		std::size_t size() const { return size_; }
		std::size_t capacity() const { return capacity_; }

		// Ensure at least n elements will fit in the pool.
		inline void expand(std::size_t n) {
			if (n >= size_) {
				if (n >= capacity_) reserve(n);
				size_ = n;
			}
		}

		inline void reserve(std::size_t n) {
			while (capacity_ < n) {
				create(chunk_size_);
				capacity_ += chunk_size_;
			}
		}

		virtual void *get(std::size_t n) = 0;

		virtual const void *get(std::size_t n) const = 0;

		virtual void destroy(std::size_t n) = 0;

		virtual void create(std::size_t count) = 0;

	protected:
		std::size_t chunk_size_ = 0;
		std::size_t size_ = 0;
		std::size_t capacity_ = 0;
	};

	/**
	* Implementation of BasePool that provides type-"safe" deconstruction of
	* elements in the pool.
	*/
	template <typename T, std::size_t ChunkSize = 512>
	class Pool : public BasePool {
	public:
		Pool() : BasePool(sizeof(T), ChunkSize) {}
		virtual ~Pool() {
			for (auto arr : datas)
				delete[] arr;
		}

		virtual void *get(std::size_t n) override {
			assert(n < size_);
			return &(datas[n / chunk_size_][n % chunk_size_]);
		}

		virtual const void *get(std::size_t n) const override {
			assert(n < size_);
			return &(datas[n / chunk_size_][n % chunk_size_]);
		}

		virtual void destroy(std::size_t n) override {
			assert(n < size_);
			*(static_cast<T*>(get(n))) = def;
		}

		virtual void create(std::size_t count) override {
			T *arr = new T[count];
			datas.push_back(arr);
		}

	protected:
		std::vector<T*> datas;
		T def;
	};

}  // namespace entityx
