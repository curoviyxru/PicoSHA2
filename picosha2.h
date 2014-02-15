/*
The MIT License (MIT)

Copyright (C) 2014 okdshin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef PICOSHA2_H
#define PICOSHA2_H
//picosha2:20140213
#include <iostream>
#include <vector>
#include <iterator>
#include <cassert>
#include <sstream>
#include <algorithm>

namespace picosha2
{
typedef unsigned long word_t;
typedef unsigned char byte_t;

namespace detail 
{
byte_t mask_8bit(byte_t x){
	return x&0xff;
}

word_t mask_32bit(word_t x){
	return x&0xffffffff;
}

const word_t add_constant[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const word_t initial_message_digest[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

word_t ch(word_t x, word_t y, word_t z){
	return (x&y)^((~x)&z);
}

word_t maj(word_t x, word_t y, word_t z){
	return (x&y)^(x&z)^(y&z);
}

word_t rotr(word_t x, std::size_t n){
	assert(n < 32);
	return mask_32bit((x>>n)|(x<<(32-n)));
}

word_t bsig0(word_t x){
	return rotr(x, 2)^rotr(x, 13)^rotr(x, 22);
}

word_t bsig1(word_t x){
	return rotr(x, 6)^rotr(x, 11)^rotr(x, 25);
}

word_t shr(word_t x, std::size_t n){
	assert(n < 32);
	return x >> n;
}

word_t ssig0(word_t x){
	return rotr(x, 7)^rotr(x, 18)^shr(x, 3);
}

word_t ssig1(word_t x){
	return rotr(x, 17)^rotr(x, 19)^shr(x, 10);
}

template<typename RaIter1, typename RaIter2>
void hash_block(RaIter1 message_digest, RaIter2 first, RaIter2 last){
	word_t w[64];
	std::fill(w, w+64, 0);
	for(std::size_t i = 0; i < 16; ++i){
		w[i] = (static_cast<word_t>(mask_8bit(*(first+i*4)))<<24)
			|(static_cast<word_t>(mask_8bit(*(first+i*4+1)))<<16) 
			|(static_cast<word_t>(mask_8bit(*(first+i*4+2)))<<8)
			|(static_cast<word_t>(mask_8bit(*(first+i*4+3)))); 
	}
	for(std::size_t i = 16; i < 64; ++i){
		w[i] = mask_32bit(ssig1(w[i-2])+w[i-7]+ssig0(w[i-15])+w[i-16]);
	}
	
	word_t a = *message_digest;
	word_t b = *(message_digest+1);
	word_t c = *(message_digest+2);
	word_t d = *(message_digest+3);
	word_t e = *(message_digest+4);
	word_t f = *(message_digest+5);
	word_t g = *(message_digest+6);
	word_t h = *(message_digest+7);
	
	for(std::size_t i = 0; i < 64; ++i){
		word_t temp1 = h+bsig1(e)+ch(e,f,g)+add_constant[i]+w[i];
		word_t temp2 = bsig0(a)+maj(a,b,c);
		h = g;
		g = f;
		f = e;
		e = mask_32bit(d+temp1);
		d = c;
		c = b;
		b = a;
		a = mask_32bit(temp1+temp2);
	}
	*message_digest += a;
	*(message_digest+1) += b;
	*(message_digest+2) += c;
	*(message_digest+3) += d;
	*(message_digest+4) += e;
	*(message_digest+5) += f;
	*(message_digest+6) += g;
	*(message_digest+7) += h;
}

}//namespace detail

template<typename RaIter, typename OutIter>
void hash256(RaIter first, RaIter last, OutIter first2, OutIter last2){
	word_t h[8];
	std::copy(detail::initial_message_digest, detail::initial_message_digest+8, h);
	std::size_t i = 0;
	std::size_t data_length = std::distance(first, last);
	for(i = 0; i+64 < data_length; i+=64){
		detail::hash_block(h, first+i, first+i+64);	
	}

	byte_t temp[64];
	std::fill(temp, temp+64, 0);
	std::size_t remains = data_length-i;
	std::copy(first+i, last, temp);
	temp[remains] = 0x80;

	if(remains > 55){
		std::fill(temp+remains+1, temp+64, 0);
		detail::hash_block(h, temp, temp+64);
		std::fill(temp, temp+64-8, 0);
	}
	else {
		std::fill(temp+remains+1, temp+64-8, 0);
	}

	std::size_t bit_length = data_length*8;
	for(std::size_t i = 0; i < 8; ++i){
		temp[56+i] = detail::mask_8bit(static_cast<byte_t>(bit_length >> (56-i*8)));
	}
	detail::hash_block(h, temp, temp+64);
	for(word_t* iter = h; iter != h+8; ++iter){
		for(std::size_t i = 0; i < 4 && first2 != last2; ++i){
			*(first2++) = detail::mask_8bit(static_cast<byte_t>((*iter >> (24-8*i))));
		}
	}
}

template<typename RaIter, typename OutContainer>
void hash256(RaIter first, RaIter last, OutContainer& dst){
	hash256(first, last, dst.begin(), dst.end());
}

template<typename RaContainer, typename OutIter>
void hash256(const RaContainer& src, OutIter first, OutIter last){
	hash256(src.begin(), src.end(), first, last);
}

template<typename RaContainer, typename OutContainer>
void hash256(const RaContainer& src, OutContainer& dst){
	hash256(src.begin(), src.end(), dst.begin(), dst.end());
}

template<typename InIter>
void output_hex(InIter first, InIter last, std::ostream& os){
	os.setf(std::ios::hex, std::ios::basefield);
	while(first != last){
		os.width(2);
		os.fill('0');
		os << static_cast<unsigned int>(*first);
		++first;
	}	
	os.setf(std::ios::dec, std::ios::basefield);
}

template<typename InIter>
void bytes_to_hex_string(InIter first, InIter last, std::string& hex_str){
	std::ostringstream oss;
	output_hex(first, last, oss);
	hex_str.assign(oss.str());
}

template<typename InContainer>
void bytes_to_hex_string(const InContainer& bytes, std::string& hex_str){
	bytes_to_hex_string(bytes.begin(), bytes.end(), hex_str);
}

template<typename InIter>
std::string bytes_to_hex_string(InIter first, InIter last){
	std::string hex_str;
	bytes_to_hex_string(first, last, hex_str);
	return hex_str;
}

template<typename InContainer>
std::string bytes_to_hex_string(const InContainer& bytes){
	std::string hex_str;
	bytes_to_hex_string(bytes, hex_str);
	return hex_str;
}

template<typename RaIter>
void hash256_hex_string(RaIter first, RaIter last, std::string& hex_str){
	byte_t hashed[32];
	hash256(first, last, hashed, hashed+32);
	std::ostringstream oss;
	output_hex(hashed, hashed+32, oss);
	hex_str.assign(oss.str());
}

template<typename RaIter>
std::string hash256_hex_string(RaIter first, RaIter last){
	std::string hex_str;
	hash256_hex_string(first, last, hex_str);
	return hex_str;
}

void hash256_hex_string(const std::string& src, std::string& hex_str){
	hash256_hex_string(src.begin(), src.end(), hex_str);
}

template<typename RaContainer>
void hash256_hex_string(const RaContainer& src, std::string& hex_str){
	hash256_hex_string(src.begin(), src.end(), hex_str);
}

template<typename RaContainer>
std::string hash256_hex_string(const RaContainer& src){
	return hash256_hex_string(src.begin(), src.end());
}

}//namespace picosha2

#endif //PICOSHA2_H