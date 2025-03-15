/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MIME.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: daong <daong@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 09:41:40 by daong             #+#    #+#             */
/*   Updated: 2025/03/15 09:41:43 by daong            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __MIME_HPP
#define __MIME_HPP

#include "AServer.hpp"

class MIME{
	private:
		MIME();
	
	public:
		~MIME();
		static const string KEY;
		static const string HTML;
		static const string XML;
		static const string CSS;
		static const string GIF;
		static const string JPEG;
		static const string JS;
		static const string ATOMXML;
		static const string RSSXML;
		static const string MATHML;
		static const string TXT;
		// image
		static const string AVIF;
		static const string PNG;
		static const string SVGXML;
		static const string TIFF;
		static const string WEBP;
		static const string ICO;
		static const string JNG;
		static const string BMP;
		// application
		static const string JSON;
		static const string PDF;
		static const string RTF;
		static const string DOC;
		static const string JAR;
		static const string Z7;
		static const string RAR;
		static const string ZIP;
		static const string BIN;
		// audio
		static const string MIDI;
		static const string MPEG;
		static const string OGG;
		static const string M4A;
		static const string REALAUDIO;
		// video
		static const string GPP;
		static const string MP2T;
		static const string MP4;
		static const string MPEGV;
		static const string QUICKTIME;
		static const string WEBM;
		static const string FLV;
		static const string M4V;
		static const string MNG;
		static const string ASF;
		static const string WMV;
		static const string AVI;

};


#endif