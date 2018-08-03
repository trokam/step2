--
-- PostgreSQL database dump
--

-- Dumped from database version 10.4 (Ubuntu 10.4-0ubuntu0.18.04)
-- Dumped by pg_dump version 10.4 (Ubuntu 10.4-0ubuntu0.18.04)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


--
-- Name: pg_trgm; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS pg_trgm WITH SCHEMA public;


--
-- Name: EXTENSION pg_trgm; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION pg_trgm IS 'text similarity measurement and index searching based on trigrams';


SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: findings; Type: TABLE; Schema: public; Owner: nikolai
--

CREATE TABLE public.findings (
    index_words integer NOT NULL,
    index_page integer NOT NULL,
    url text,
    title text,
    relevance_in_body integer,
    relevance_in_url integer,
    relevance_in_title integer,
    relevance_total integer,
    snippet text
);


ALTER TABLE public.findings OWNER TO nikolai;

--
-- Name: words; Type: TABLE; Schema: public; Owner: nikolai
--

CREATE TABLE public.words (
    index integer NOT NULL,
    value text,
    count integer,
    tokens tsvector
);


ALTER TABLE public.words OWNER TO nikolai;

--
-- Name: findings findings_pkey; Type: CONSTRAINT; Schema: public; Owner: nikolai
--

ALTER TABLE ONLY public.findings
    ADD CONSTRAINT findings_pkey PRIMARY KEY (index_words, index_page);


--
-- Name: words words_pkey; Type: CONSTRAINT; Schema: public; Owner: nikolai
--

ALTER TABLE ONLY public.words
    ADD CONSTRAINT words_pkey PRIMARY KEY (index);


--
-- Name: words_count; Type: INDEX; Schema: public; Owner: nikolai
--

CREATE INDEX words_count ON public.words USING btree (count);


--
-- Name: words_tokens; Type: INDEX; Schema: public; Owner: nikolai
--

CREATE INDEX words_tokens ON public.words USING gin (tokens);


--
-- Name: words_value_trgm; Type: INDEX; Schema: public; Owner: nikolai
--

CREATE INDEX words_value_trgm ON public.words USING gin (value public.gin_trgm_ops);


--
-- PostgreSQL database dump complete
--

