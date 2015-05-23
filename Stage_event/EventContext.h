﻿#ifndef EVENTCONTEXT_H
#define EVENTCONTEXT_H

#include "stdafx.h"
#include <functional>
#include <iostream>
#include "ContextVarList.h"

namespace stage{
	/** Luokka, joka ylläpitää kontekstitietoja usean viestin käsittelyn yli.
	Mahdollistaa "lokaalien" muuttujien käsittelemisen useasta toisiinsa liittyvästä viestinkäsittelijästä.
	*/
	class EventContext{
	public:
		/** Tähän kontekstiin liittyvien vastausviestien lukumäärä.
		Esim. jos kontekstin yhteydessä lähetetään viestit kolmelle aktorille, responseCount:n arvoksi asetetaan 3,
		jotta lopetusmetodi suoritetaan, kun kaikki ovat vastanneet
		*/
		unsigned int responseCount;

		/** Kontekstin lopetusmetodi. Suoritetaan automaattisesti, kun kaikki odotetut vastaukset ovat saapuneet.
		*/
		std::function<void()> finalize;

		/** Kontekstin virhemetodi. Suoritetaan, jos kontekstiin liittyen lähetettyjen viestien käsittelyssä tapahtuu virhe.
		*/
		std::function<void()> error;

		ContextVar* varHead = nullptr;

		//TODO: mielivaltaiset muuttujat boost::any:n avulla

		/** Luo uuden tapahtumakontekstin.
		@param id				Sen viestin tunnusluku, jonka vuoksi tämä konteksti luotiin
		@param sender			Sen viestin lähettäjä, jonka vuoksi tämä konteksti luotiin
		@param responseCount	Tähän viestiin odotettujen vastausten määrä
		*/
		EventContext(uint64_t id, Theron::Address sender, unsigned int responseCount): originalID(id), originalSender(sender), 
			responseCount(responseCount){}

		/** Luo tyhjän tapahtumakontekstin kontekstilistan tietorakenteiden initialisointia varten
		*/
		EventContext() : originalID(0), originalSender(0), responseCount(0){
			//Estetään lopetusmetodin kutsuminen tyhjälle kontekstille
			finalize = [](){abort(); };
			error = finalize;
		}

		EventContext(EventContext& other) = delete;

		~EventContext(){
			if (varHead != nullptr){
				delete varHead;
			}
		}

		boost::any getVar(int depth) const{
			return varHead->get(depth);
		}

		void setVar(int depth, boost::any var){
			if (varHead == nullptr){
				boost::any a = 0;
				varHead = new ContextVar(a);
			}
			varHead->set(depth, var);
		}

		/** Palauttaa sen viestin tunnuksen, joka aiheutti tämän kontekstin luomisen
		@returns	alkuperäinen viestitunnus
		*/
		uint64_t getOriginalID() const { return originalID; }

		/** Palauttaa sen viestin lähettäjän, joka aiheutti tämän kontekstin luomisen
		@returns	alkuperäisen lähettäjän Theron-osoite
		*/
		Theron::Address getOriginalSender() const { return originalSender; }
	private:
		/** Sen viestin tunnus, joka aiheutti tämän kontekstin luomisen
		*/
		uint64_t originalID;

		/** Sen viestin lähettäjä, joka aiheutti tämän kontekstin luomisen
		*/
		Theron::Address originalSender;
	};

}

#endif