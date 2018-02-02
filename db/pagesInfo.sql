-- Database generated with pgModeler (PostgreSQL Database Modeler).
-- pgModeler  version: 0.8.1
-- PostgreSQL version: 9.4
-- Project Site: pgmodeler.com.br
-- Model Author: ---


-- Database creation must be done outside an multicommand file.
-- These commands were put in this file only for convenience.
-- -- object: new_database | type: DATABASE --
-- -- DROP DATABASE IF EXISTS new_database;
-- CREATE DATABASE new_database
-- ;
-- -- ddl-end --
-- 

-- object: public.sequences | type: TABLE --
-- DROP TABLE IF EXISTS public.sequences CASCADE;
CREATE TABLE public.sequences(
	index serial NOT NULL,
	value varchar(1000) NOT NULL,
	CONSTRAINT text_pkey PRIMARY KEY (index),
	CONSTRAINT value_unique UNIQUE (value)

);
-- ddl-end --
ALTER TABLE public.sequences OWNER TO postgres;
-- ddl-end --

-- object: public.page | type: TABLE --
-- DROP TABLE IF EXISTS public.page CASCADE;
CREATE TABLE public.page(
	index serial NOT NULL,
	protocol varchar(10) NOT NULL,
	domain varchar(250) NOT NULL,
	path varchar(2500) NOT NULL,
	level integer NOT NULL,
	processing bool NOT NULL,
	crunched date NOT NULL,
	CONSTRAINT page_pkey PRIMARY KEY (index)

);
-- ddl-end --
ALTER TABLE public.page OWNER TO postgres;
-- ddl-end --

-- object: public.page_seq | type: TABLE --
-- DROP TABLE IF EXISTS public.page_seq CASCADE;
CREATE TABLE public.page_seq(
	index_page integer,
	index_sequences integer,
	count integer NOT NULL
);
-- ddl-end --
ALTER TABLE public.page_seq OWNER TO postgres;
-- ddl-end --

-- object: page_fk | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS page_fk CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT page_fk FOREIGN KEY (index_page)
REFERENCES public.page (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: sequences_fk | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS sequences_fk CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT sequences_fk FOREIGN KEY (index_sequences)
REFERENCES public.sequences (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: unique_page_text | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS unique_page_text CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT unique_page_text UNIQUE (index_page,index_sequences);
-- ddl-end --

-- object: public.global_occ | type: TABLE --
-- DROP TABLE IF EXISTS public.global_occ CASCADE;
CREATE TABLE public.global_occ(
	count integer,
	index_sequences integer
);
-- ddl-end --
ALTER TABLE public.global_occ OWNER TO postgres;
-- ddl-end --

-- object: sequences_fk | type: CONSTRAINT --
-- ALTER TABLE public.global_occ DROP CONSTRAINT IF EXISTS sequences_fk CASCADE;
ALTER TABLE public.global_occ ADD CONSTRAINT sequences_fk FOREIGN KEY (index_sequences)
REFERENCES public.sequences (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: public.content | type: TABLE --
-- DROP TABLE IF EXISTS public.content CASCADE;
CREATE TABLE public.content(
	index_page integer,
	value text
);
-- ddl-end --
ALTER TABLE public.content OWNER TO postgres;
-- ddl-end --

-- object: page_fk | type: CONSTRAINT --
-- ALTER TABLE public.content DROP CONSTRAINT IF EXISTS page_fk CASCADE;
ALTER TABLE public.content ADD CONSTRAINT page_fk FOREIGN KEY (index_page)
REFERENCES public.page (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: content_uq | type: CONSTRAINT --
-- ALTER TABLE public.content DROP CONSTRAINT IF EXISTS content_uq CASCADE;
ALTER TABLE public.content ADD CONSTRAINT content_uq UNIQUE (index_page);
-- ddl-end --


