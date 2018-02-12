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

-- object: public.sequence | type: TABLE --
-- DROP TABLE IF EXISTS public.sequence CASCADE;
CREATE TABLE public.sequence(
	index serial NOT NULL,
	value varchar(1000) NOT NULL,
	CONSTRAINT text_pkey PRIMARY KEY (index),
	CONSTRAINT value_unique UNIQUE (value)

);
-- ddl-end --
ALTER TABLE public.sequence OWNER TO postgres;
-- ddl-end --

-- object: public.page | type: TABLE --
-- DROP TABLE IF EXISTS public.page CASCADE;
CREATE TABLE public.page(
	index serial NOT NULL,
	protocol varchar(10),
	domain varchar(250),
	path varchar(1000),
	level integer NOT NULL,
	processing bool NOT NULL,
	crunched date NOT NULL,
	state smallint NOT NULL,
	type varchar(200) NOT NULL,
	CONSTRAINT page_pkey PRIMARY KEY (index),
	CONSTRAINT page_unique UNIQUE (protocol,domain,path)

);
-- ddl-end --
ALTER TABLE public.page OWNER TO postgres;
-- ddl-end --

-- object: public.page_seq | type: TABLE --
-- DROP TABLE IF EXISTS public.page_seq CASCADE;
CREATE TABLE public.page_seq(
	index_page integer,
	index_sequence integer,
	count integer NOT NULL,
	relevance float NOT NULL
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

-- object: sequence_fk | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS sequence_fk CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT sequence_fk FOREIGN KEY (index_sequence)
REFERENCES public.sequence (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: unique_page_text | type: CONSTRAINT --
-- ALTER TABLE public.page_seq DROP CONSTRAINT IF EXISTS unique_page_text CASCADE;
ALTER TABLE public.page_seq ADD CONSTRAINT unique_page_text UNIQUE (index_page,index_sequence);
-- ddl-end --

-- object: public.trait | type: TABLE --
-- DROP TABLE IF EXISTS public.trait CASCADE;
CREATE TABLE public.trait(
	index_page integer,
	title varchar(1000) NOT NULL,
	content_size integer NOT NULL,
	complexity float NOT NULL
);
-- ddl-end --
ALTER TABLE public.trait OWNER TO postgres;
-- ddl-end --

-- object: page_fk | type: CONSTRAINT --
-- ALTER TABLE public.trait DROP CONSTRAINT IF EXISTS page_fk CASCADE;
ALTER TABLE public.trait ADD CONSTRAINT page_fk FOREIGN KEY (index_page)
REFERENCES public.page (index) MATCH FULL
ON DELETE SET NULL ON UPDATE CASCADE;
-- ddl-end --

-- object: trait_uq | type: CONSTRAINT --
-- ALTER TABLE public.trait DROP CONSTRAINT IF EXISTS trait_uq CASCADE;
ALTER TABLE public.trait ADD CONSTRAINT trait_uq UNIQUE (index_page);
-- ddl-end --


